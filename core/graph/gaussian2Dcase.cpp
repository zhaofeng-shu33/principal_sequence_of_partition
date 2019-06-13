#include "core/graph/gaussian2Dcase.h"
namespace demo {
	Gaussian2DGraphBase::Gaussian2DGraphBase(int np, double gamma, double a[][2]) :
		distribution(0, 1),
		_gamma(gamma)
	{
		if (a == NULL) {
			// generator num_points 2D points ~ N(0,1) located at different positions
			unsigned seed = std::chrono::system_clock::now().time_since_epoch().count();
			generator = std::default_random_engine(seed);

			for (int j = 0; j < 4; j++) {
				for (int i = 0; i < np / 4; i++) {
					x_pos.push_back(distribution(generator) + data_1[j][0]);
					y_pos.push_back(distribution(generator) + data_1[j][1]);
				}
			}
		}
		else {
			for (int j = 0; j < np; j++) {
				x_pos.push_back(a[j][0]);
				y_pos.push_back(a[j][1]);
			}
		}
		for (int i = 0; i < np; i++)
			for (int j = i + 1; j < np; j++) {
				edge_list_tuple.push_back(std::make_tuple(i, j, compute_similarity(x_pos[i], y_pos[i], x_pos[j], y_pos[j])));
			}
	}
	EdgeListTuple Gaussian2DGraphBase::get_edge_list_tuple() { return edge_list_tuple; }
	double Gaussian2DGraphBase::compute_similarity(double x_1, double y_1, double x_2, double y_2) {
		return exp(-1.0 * _gamma * pow(x_1 - x_2, 2) - _gamma * pow(y_1 - y_2, 2));
	}

	Gaussian2DGraph::Gaussian2DGraph(int np, double gamma, double a[][2]) :
		Gaussian2DGraphBase(np, gamma, a),
		InfoCluster(edge_list_tuple, np)
	{
	}
	std::vector<double> Gaussian2DGraph::get_x_pos_list() {
		return x_pos;
	}
	std::vector<double> Gaussian2DGraph::get_y_pos_list() {
		return y_pos;
	}
}