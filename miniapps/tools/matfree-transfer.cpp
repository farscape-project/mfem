#include "mfem.hpp"
#include "../../fem/transfer.hpp"
#include <fstream>
#include <iostream>

using namespace std;
using namespace mfem;

int problem = 1; // problem type

int Wx = 0, Wy = 0; // window position
int Ww = 350, Wh = 350; // window size
int offx = Ww+5, offy = Wh+25; // window offsets

string space;
string direction;

// Exact functions to project
real_t RHO_exact(const Vector &x);

// Helper functions
void visualize(VisItDataCollection &, string, int, int);
real_t compute_mass(FiniteElementSpace *, real_t, VisItDataCollection &,
                    string);

int main(int argc, char *argv[])
{
   // Parse command-line options.
   const char *mesh_file = "../../data/star.mesh";
   int order = 3;
   int lref = order+1;
   int lorder = 0;
   bool vis = true;
   bool useH1 = false;
   bool use_pointwise_transfer = false;
   const char *device_config = "cpu";

   OptionsParser args(argc, argv);
   args.AddOption(&mesh_file, "-m", "--mesh",
                  "Mesh file to use.");
   args.AddOption(&problem, "-p", "--problem",
                  "Problem type (see the RHO_exact function).");
   args.AddOption(&order, "-o", "--order",
                  "Finite element order (polynomial degree) or -1 for"
                  " isoparametric space.");
   args.AddOption(&lref, "-lref", "--lor-ref-level", "LOR refinement level.");
   args.AddOption(&lorder, "-lo", "--lor-order",
                  "LOR space order (polynomial degree, zero by default).");
   args.AddOption(&vis, "-vis", "--visualization", "-no-vis",
                  "--no-visualization",
                  "Enable or disable GLVis visualization.");
   args.AddOption(&useH1, "-h1", "--use-h1", "-l2", "--use-l2",
                  "Use H1 spaces instead of L2.");
   args.AddOption(&use_pointwise_transfer, "-t", "--use-pointwise-transfer",
                  "-no-t", "--dont-use-pointwise-transfer",
                  "Use pointwise transfer operators instead of L2 projection.");
   args.AddOption(&device_config, "-d", "--device",
                  "Device configuration string, see Device::Configure().");
   args.ParseCheck();

   // Read the mesh from the given mesh file.
   Mesh mesh(mesh_file, 1, 1);
   // cout << "Element type is " << mesh.GetElementType(0) << endl;
   int dim = mesh.Dimension();

   // Create the low-order refined mesh
   int basis_lor = BasisType::GaussLobatto; // BasisType::ClosedUniform;
   Mesh mesh_lor = Mesh::MakeRefined(mesh, lref, basis_lor);

   // Create spaces
   FiniteElementCollection *fec, *fec_lor;
   if (useH1)
   {
      space = "H1";
      if (lorder == 0)
      {
         lorder = 1;
         cerr << "Switching the H1 LOR space order from 0 to 1\n";
      }
      fec = new H1_FECollection(order, dim);
      fec_lor = new H1_FECollection(lorder, dim);
   }
   else
   {
      space = "L2";
      fec = new L2_FECollection(order, dim);
      fec_lor = new L2_FECollection(lorder, dim);
   }

   FiniteElementSpace fespace(&mesh, fec);
   FiniteElementSpace fespace_lor(&mesh_lor, fec_lor);

   // Build the integration rule that matches with quadrature on mixed mass matrix,
   // assuming HO elements are the same, and that all HO are LOR in the same way
   Geometry::Type geom = mesh.GetElementBaseGeometry(0);
   const FiniteElement &fe = *fespace.GetFE(0);
   const FiniteElement &fe_lor = *fespace_lor.GetFE(0);
   ElementTransformation *el_tr = fespace_lor.GetElementTransformation(0);
   int qorder = fe_lor.GetOrder() + fe.GetOrder() + el_tr->OrderW(); // 0 + 3 + 1
   const IntegrationRule* ir = &IntRules.Get(geom, qorder);

   QuadratureSpace qspace(mesh_lor, *ir);
   QuadratureFunction qfunc(&qspace);
   qfunc = 333.0;
   // qfunc(2) = 7; // does not pass verify_solution
   // qfunc(7) = 333.000001; // does not pass verify_solution
   qfunc(7) = 333.0000001; // passes verify_solution
   QuadratureFunctionCoefficient coeff(qfunc);

   GridFunction rho(&fespace);
   GridFunction rho_lor(&fespace_lor);

   // Data collections for vis/analysis
   VisItDataCollection HO_dc("HO", &mesh);
   HO_dc.RegisterField("density", &rho);
   VisItDataCollection LOR_dc("LOR", &mesh_lor);
   LOR_dc.RegisterField("density", &rho_lor);

   BilinearForm M_ho(&fespace);
   M_ho.AddDomainIntegrator(new MassIntegrator);
   M_ho.Assemble();
   M_ho.Finalize();

   BilinearForm M_lor(&fespace_lor);
   M_lor.AddDomainIntegrator(new MassIntegrator);
   M_lor.Assemble();
   M_lor.Finalize();

   // HO projections
   direction = "HO -> LOR @ HO";
   FunctionCoefficient RHO(RHO_exact);
   rho.ProjectCoefficient(RHO);
   // Make sure AMR constraints are satisfied
   rho.SetTrueVector();
   rho.SetFromTrueVector();

   real_t ho_mass = compute_mass(&fespace, -1.0, HO_dc, "HO       ");
   if (vis) { visualize(HO_dc, "HO", Wx, Wy); Wx += offx; }

   GridTransfer *gt;
   if (use_pointwise_transfer)
   {
      gt = new InterpolationGridTransfer(fespace, fespace_lor);
   }
   else
   {
      gt = new L2ProjectionGridTransfer(fespace, fespace_lor, &coeff);
   }

   gt->UseDevice(true);
   gt->VerifySolution(true);

   const Operator &R = gt->ForwardOperator();
   TransferOperator to(fespace_lor, fespace);

   // HO->LOR restriction
   direction = "HO -> LOR @ LOR";
   R.Mult(rho, rho_lor);
   compute_mass(&fespace_lor, ho_mass, LOR_dc, "R(HO)    ");
   if (vis) { visualize(LOR_dc, "R(HO)", Wx, Wy); Wx += offx; }



   if (gt->SupportsBackwardsOperator())
   {
      const Operator &P = gt->BackwardOperator();
      // LOR->HO prolongation
      direction = "HO -> LOR @ HO";
      GridFunction rho_prev = rho;
      P.Mult(rho_lor, rho);
      //   opr->Mult(rho_lor, rho);
      compute_mass(&fespace, ho_mass, HO_dc, "P(R(HO)) ");
      if (vis) { visualize(HO_dc, "P(R(HO))", Wx, Wy); Wx = 0; Wy += offy; }

      rho_prev -= rho;
      cout.precision(12);
      cout << "|HO - P(R(HO))|_∞   = " << rho_prev.Normlinf() << endl;
   }

   // HO* to LOR* dual fields
   LinearForm M_rho(&fespace), M_rho_lor(&fespace_lor);
   if (!use_pointwise_transfer && gt->SupportsBackwardsOperator())
   {
      const Operator &P = gt->BackwardOperator();
      M_ho.Mult(rho, M_rho);
      P.MultTranspose(M_rho, M_rho_lor);
      cout << "HO -> LOR dual field: " << abs(M_rho.Sum()-M_rho_lor.Sum()) << "\n\n";
   }

   // LOR projections
   direction = "LOR -> HO @ LOR";
   rho_lor.ProjectCoefficient(RHO);
   GridFunction rho_lor_prev = rho_lor;
   real_t lor_mass = compute_mass(&fespace_lor, -1.0, LOR_dc, "LOR      ");
   if (vis) { visualize(LOR_dc, "LOR", Wx, Wy); Wx += offx; }

   if (gt->SupportsBackwardsOperator())
   {
      const Operator &P = gt->BackwardOperator();
      // Prolongate to HO space
      direction = "LOR -> HO @ HO";
      P.Mult(rho_lor, rho);
      compute_mass(&fespace, lor_mass, HO_dc, "P(LOR)   ");
      if (vis) { visualize(HO_dc, "P(LOR)", Wx, Wy); Wx += offx; }

      // Restrict back to LOR space. This won't give the original function because
      // the rho_lor doesn't necessarily live in the range of R.
      direction = "LOR -> HO @ LOR";
      R.Mult(rho, rho_lor);
      compute_mass(&fespace_lor, lor_mass, LOR_dc, "R(P(LOR))");
      if (vis) { visualize(LOR_dc, "R(P(LOR))", Wx, Wy); }

      rho_lor_prev -= rho_lor;
      cout.precision(12);
      cout << "|LOR - R(P(LOR))|_∞ = " << rho_lor_prev.Normlinf() << endl;
   }

   // LOR* to HO* dual fields
   if (!use_pointwise_transfer)
   {
      M_lor.Mult(rho_lor, M_rho_lor);
      R.MultTranspose(M_rho_lor, M_rho);
      cout << "LOR -> HO dual field: " << abs(M_rho.Sum() - M_rho_lor.Sum()) << '\n';
   }

   delete fec;
   delete fec_lor;

   // RandomRefinement(fespace);

   return 0;
}


real_t RHO_exact(const Vector &x)
{
   switch (problem)
   {
      case 1: // smooth field
         return x(1)+0.25*cos(2*M_PI*x.Norml2());
      case 2: // cubic function
         return x(1)*x(1)*x(1) + 2*x(0)*x(1) + x(0);
      case 3: // sharp gradient
         return M_PI/2-atan(5*(2*x.Norml2()-1));
      case 4: // basis function
         return (x.Norml2() < 0.1) ? 1 : 0;
      default:
         return 1.0;
   }
}


void visualize(VisItDataCollection &dc, string prefix, int x, int y)
{
   int w = Ww, h = Wh;

   char vishost[] = "localhost";
   int  visport   = 19916;

   socketstream sol_sockL2(vishost, visport);
   sol_sockL2.precision(8);
   sol_sockL2 << "solution\n" << *dc.GetMesh() << *dc.GetField("density")
              << "window_geometry " << x << " " << y << " " << w << " " << h
              << "plot_caption '" << space << " " << prefix << " Density'"
              << "window_title '" << direction << "'" << flush;
}


real_t compute_mass(FiniteElementSpace *L2, real_t massL2,
                    VisItDataCollection &dc, string prefix)
{
   ConstantCoefficient one(1.0);
   LinearForm lf(L2);
   lf.AddDomainIntegrator(new DomainLFIntegrator(one));
   lf.Assemble();

   real_t newmass = lf(*dc.GetField("density"));
   cout.precision(18);
   cout << space << " " << prefix << " mass   = " << newmass;
   if (massL2 >= 0)
   {
      cout.precision(4);
      cout << " ("  << fabs(newmass-massL2)*100/massL2 << "%)";
   }
   cout << endl;
   return newmass;
}
