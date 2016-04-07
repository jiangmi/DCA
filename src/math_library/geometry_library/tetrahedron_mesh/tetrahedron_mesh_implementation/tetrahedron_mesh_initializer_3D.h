//-*-C++-*-

#ifndef MATH_LIBRARY_GEOMETRY_LIBRARY_TETRAHEDRON_MESH_TETRAHEDRON_MESH_IMPLEMENTATION_TETRAHEDRON_MESH_INITIALIZER_3D_H
#define MATH_LIBRARY_GEOMETRY_LIBRARY_TETRAHEDRON_MESH_TETRAHEDRON_MESH_IMPLEMENTATION_TETRAHEDRON_MESH_INITIALIZER_3D_H

namespace math_algorithms {
/*
 *  \author: peter staar
 */
template <class k_cluster_type>
class tetrahedron_mesh_initializer<3, k_cluster_type> {
  const static int DIMENSION = 3;

  typedef tetrahedron<3> tetrahedron_t;
  typedef simplex<3> simplex_t;
  typedef facet<3> facet_t;
  typedef std::vector<double> vector_t;

public:
  tetrahedron_mesh_initializer(std::vector<simplex_t>& simplices_ref,
                               std::vector<facet_t>& facets_ref, std::vector<vector_t>& mesh_ref,
                               std::vector<tetrahedron_t>& tetrahedra_ref, int N_recursion_ref);

  ~tetrahedron_mesh_initializer();

  void execute();

private:
  void make_convex_hull();
  void find_facets();
  void make_mesh_points();

private:
  std::vector<simplex_t>& simplices;
  std::vector<facet_t>& facets;

  std::vector<vector_t>& mesh;
  std::vector<tetrahedron_t>& tetrahedra;

  int N_recursion;
};

template <class k_cluster_type>
tetrahedron_mesh_initializer<3, k_cluster_type>::tetrahedron_mesh_initializer(
    std::vector<simplex_t>& simplices_ref, std::vector<facet_t>& facets_ref,
    std::vector<vector_t>& mesh_ref, std::vector<tetrahedron_t>& tetrahedra_ref, int N_recursion_ref)
    : simplices(simplices_ref),
      facets(facets_ref),
      mesh(mesh_ref),
      tetrahedra(tetrahedra_ref),

      N_recursion(N_recursion_ref) {}

template <class k_cluster_type>
tetrahedron_mesh_initializer<3, k_cluster_type>::~tetrahedron_mesh_initializer() {}

template <class k_cluster_type>
void tetrahedron_mesh_initializer<3, k_cluster_type>::execute() {
  make_convex_hull();

  find_facets();

  make_mesh_points();
}

template <class k_cluster_type>
void tetrahedron_mesh_initializer<3, k_cluster_type>::make_convex_hull() {
  std::vector<double>& b0 = k_cluster_type::get_basis_vectors()[0];
  std::vector<double>& b1 = k_cluster_type::get_basis_vectors()[1];
  std::vector<double>& b2 = k_cluster_type::get_basis_vectors()[2];

  std::vector<std::vector<double>> B_collection(0);
  {
    std::vector<double> B(3, 0);

    for (int t0 = -1; t0 <= 1; t0++) {
      for (int t1 = -1; t1 <= 1; t1++) {
        for (int t2 = -1; t2 <= 1; t2++) {
          if (t0 != 0 || t1 != 0 || t2 != 0) {
            B[0] = t0 * b0[0] + t1 * b1[0] + t2 * b2[0];
            B[1] = t0 * b0[1] + t1 * b1[1] + t2 * b2[1];
            B[2] = t0 * b0[2] + t1 * b1[2] + t2 * b2[2];

            B_collection.push_back(B);
          }
        }
      }
    }
  }

  double* A = new double[3 * 3];
  double* B = new double[3];

  solve_plan<double> slv_pln(3, 1);

  for (size_t l0 = 0; l0 < B_collection.size(); l0++) {
    for (size_t l1 = 0; l1 < B_collection.size(); l1++) {
      for (size_t l2 = 0; l2 < B_collection.size(); l2++) {
        A[0 + 3 * 0] = B_collection[l0][0];
        A[0 + 3 * 1] = B_collection[l0][1];
        A[0 + 3 * 2] = B_collection[l0][2];
        A[1 + 3 * 0] = B_collection[l1][0];
        A[1 + 3 * 1] = B_collection[l1][1];
        A[1 + 3 * 2] = B_collection[l1][2];
        A[2 + 3 * 0] = B_collection[l2][0];
        A[2 + 3 * 1] = B_collection[l2][1];
        A[2 + 3 * 2] = B_collection[l2][2];

        B[0] = B_collection[l0][0] * B_collection[l0][0] / 2. +
               B_collection[l0][1] * B_collection[l0][1] / 2. +
               B_collection[l0][2] * B_collection[l0][2] / 2.;
        B[1] = B_collection[l1][0] * B_collection[l1][0] / 2. +
               B_collection[l1][1] * B_collection[l1][1] / 2. +
               B_collection[l1][2] * B_collection[l1][2] / 2.;
        B[2] = B_collection[l2][0] * B_collection[l2][0] / 2. +
               B_collection[l2][1] * B_collection[l2][1] / 2. +
               B_collection[l2][2] * B_collection[l2][2] / 2.;

        {
          memcpy(slv_pln.matrix, A, sizeof(double) * 9);
          memcpy(slv_pln.solved_matrix, B, sizeof(double) * 3);

          slv_pln.execute_plan();

          double det_A = slv_pln.matrix[0] * slv_pln.matrix[4] * slv_pln.matrix[8];

          if (std::fabs(det_A) > 1.e-6) {
            simplex<DIMENSION> s;
            s.k_vec.resize(3, 0);

            s.k_vec[0] = slv_pln.solved_matrix[0];
            s.k_vec[1] = slv_pln.solved_matrix[1];
            s.k_vec[2] = slv_pln.solved_matrix[2];

            simplices.push_back(s);
          }
        }
      }
    }
  }

  delete[] A;
  delete[] B;

  {
    std::vector<double> K(3, 0);

    for (size_t B_ind = 0; B_ind < B_collection.size(); B_ind++) {
      for (size_t s_ind = 0; s_ind < simplices.size(); s_ind++) {
        double diff_k_K = VECTOR_OPERATIONS::L2_NORM(simplices[s_ind].k_vec, K);
        double diff_k_B = VECTOR_OPERATIONS::L2_NORM(simplices[s_ind].k_vec, B_collection[B_ind]);

        if (diff_k_K > diff_k_B + 1.e-6) {
          simplices.erase(simplices.begin() + s_ind);
          s_ind--;
        }
      }
    }
  }

  {
    for (size_t s_ind0 = 0; s_ind0 < simplices.size(); s_ind0++) {
      for (size_t s_ind1 = s_ind0 + 1; s_ind1 < simplices.size(); s_ind1++) {
        if (VECTOR_OPERATIONS::L2_NORM(simplices[s_ind0].k_vec, simplices[s_ind1].k_vec) < 1.e-6) {
          simplices.erase(simplices.begin() + s_ind1);
          s_ind1--;
        }
      }
    }
  }
}

template <class k_cluster_type>
void tetrahedron_mesh_initializer<3, k_cluster_type>::find_facets() {
  int coor[3];

  for (size_t l0 = 0; l0 < simplices.size(); l0++) {
    for (size_t l1 = 0; l1 < simplices.size(); l1++) {
      for (size_t l2 = 0; l2 < simplices.size(); l2++) {
        coor[0] = l0;
        coor[1] = l1;
        coor[2] = l2;

        if (facet<DIMENSION>::is_facet(coor, simplices)) {
          facet<DIMENSION> f0;
          f0.index.push_back(l0);
          f0.index.push_back(l1);
          f0.index.push_back(l2);

          facets.push_back(f0);
        }
      }
    }
  }

  for (size_t l0 = 0; l0 < facets.size(); l0++) {
    for (size_t l1 = l0 + 1; l1 < facets.size(); l1++) {
      bool equal = facet<DIMENSION>::equal(facets[l0], facets[l1], simplices);

      if (equal) {
        for (size_t l = 0; l < facets[l1].index.size(); l++)
          facets[l0].index.push_back(facets[l1].index[l]);

        facets.erase(facets.begin() + l1);
        l1--;
      }
    }
  }

  for (size_t l0 = 0; l0 < facets.size(); l0++) {
    sort(facets[l0].index.begin(), facets[l0].index.end());
    std::vector<int>::iterator it = unique(facets[l0].index.begin(), facets[l0].index.end());
    facets[l0].index.erase(it, facets[l0].index.end());
  }
}

template <class k_cluster_type>
void tetrahedron_mesh_initializer<3, k_cluster_type>::make_mesh_points() {
  assert(DIMENSION == 3);

  std::vector<int>::iterator result_i, result_j;

  mesh.resize(1, std::vector<double>(3, 0.));

  for (size_t l = 0; l < facets.size(); l++) {
    std::vector<double> k(3, 0.);

    for (size_t i = 0; i < facets[l].index.size(); i++) {
      k[0] += simplices[facets[l].index[i]].k_vec[0] / double(facets[l].index.size());
      k[1] += simplices[facets[l].index[i]].k_vec[1] / double(facets[l].index.size());
      k[2] += simplices[facets[l].index[i]].k_vec[2] / double(facets[l].index.size());
    }

    mesh.push_back(k);
  }

  for (size_t l = 0; l < simplices.size(); l++)
    mesh.push_back(simplices[l].k_vec);

  for (size_t l = 0; l < facets.size(); l++) {
    for (size_t i = 0; i < facets[l].index.size(); i++) {
      for (size_t j = i + 1; j < facets[l].index.size(); j++) {
        bool form_edge_line = false;

        for (size_t k = 0; k < facets.size(); k++)
          if (k != l) {
            result_i = find(facets[k].index.begin(), facets[k].index.end(), facets[l].index[i]);
            result_j = find(facets[k].index.begin(), facets[k].index.end(), facets[l].index[j]);

            bool i_is_element_of_facet_k = result_i == facets[k].index.end() ? false : true;
            bool j_is_element_of_facet_k = result_j == facets[k].index.end() ? false : true;

            if (i_is_element_of_facet_k && j_is_element_of_facet_k)
              form_edge_line = true;
          }

        if (form_edge_line) {
          tetrahedron<DIMENSION> tet;

          {
            tet.index[0] = 0;
            tet.index[1] = l + 1;
            tet.index[2] = facets[l].index[i] + 1 + facets.size();
            tet.index[3] = facets[l].index[j] + 1 + facets.size();
          }

          {
            std::vector<double> normal(3, 0.);

            for (size_t i = 0; i < facets[l].index.size(); i++) {
              normal[0] += simplices[facets[l].index[i]].k_vec[0] / double(facets[l].index.size());
              normal[1] += simplices[facets[l].index[i]].k_vec[1] / double(facets[l].index.size());
              normal[2] += simplices[facets[l].index[i]].k_vec[2] / double(facets[l].index.size());
            }

            tet.normal = normal;
          }
          tetrahedra.push_back(tet);
        }
      }
    }
  }

  tetrahedra.reserve(int(tetrahedra.size()) * int(std::pow(8., N_recursion)));
  tetrahedra.reserve(int(4 * tetrahedra.size()) * int(std::pow(2., N_recursion)));

  for (int i = 0; i < N_recursion; i++) {
    int n_tet = tetrahedra.size();
    for (int l = 0; l < n_tet; l++)
      tetrahedra[l].do_recursion(tetrahedra, mesh);

    tetrahedra.erase(tetrahedra.begin(), tetrahedra.begin() + n_tet);
  }

  {  // get rid of mesh-redundancy
    std::vector<std::vector<double>>::iterator it;
    std::vector<std::vector<double>> mesh_old = mesh;

    sort(mesh.begin(), mesh.end());
    it = unique(mesh.begin(), mesh.end());
    mesh.erase(it, mesh.end());

    std::vector<int> index(mesh_old.size(), -1);

    for (size_t i = 0; i < mesh_old.size(); i++) {
      it = lower_bound(mesh.begin(), mesh.end(), mesh_old[i]);  // --> complexity log(N) !
      index[i] = it - mesh.begin();
      assert(index[i] < int(mesh.size()));
    }

    for (size_t l = 0; l < tetrahedra.size(); l++)
      for (int z = 0; z < 3 + 1; z++)
        tetrahedra[l].index[z] = index[tetrahedra[l].index[z]];
  }
}
}

#endif  // MATH_LIBRARY_GEOMETRY_LIBRARY_TETRAHEDRON_MESH_TETRAHEDRON_MESH_IMPLEMENTATION_TETRAHEDRON_MESH_INITIALIZER_3D_H
