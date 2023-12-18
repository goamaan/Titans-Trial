// internal
#include "ai_system.hpp"
#include <unordered_set>
#include <queue>
#include <map>

// 30 X 20 = last , 24 X 16
const float WIDTH = 24.f;
const float HEIGHT = 16.f;
const vec2 next[] = { vec2(0,1), vec2(0,-1), vec2(1,0), vec2(-1,0) };
//const vec2 next[] = { vec2(0,1), vec2(0,-1), vec2(1,0), vec2(-1,0) , vec2(1, 1), vec2(-1, -1), vec2(1, -1) , vec2(-1, 1) };


struct Grid_square {
	vec2 pos;
	vec2 came_from;
	float g;
	float f;
};

struct LessThanByF
{
	bool operator()(const Grid_square& comp1, const Grid_square& comp2) const {
		return comp1.f < comp2.f;
	}
};

struct cmpVec2 {
	bool operator()(const vec2 a, const vec2 b) const {
		return a.x == b.x && a.y == b.y;
	}
};


void AISystem::step(float elapsed_ms)
{
	(void)elapsed_ms; // placeholder to silence unused warning until implemented
}

void point_checker(vec2& point, float x_stop, float y_stop) {
	point.x = max(0.f, point.x);
	point.x = min(point.x, (float)x_stop - 1.f);
	point.y = max(0.f, point.y);
	point.y = min(point.y, (float)y_stop - 1.f);
}

vec2 find_map_index(vec2 pos) {
	//float h_squares_per_cube = (int)window_height_px / HEIGHT;
	//float w_squares_per_cube = (int)window_width_px / WIDTH;
	//printf("Xbefore: %f, %f==========", pos.x, pos.y);
	pos.x = round(pos.x / (window_width_px / WIDTH));
	pos.y = round(pos.y / (window_height_px / HEIGHT));

	//printf("XAFTER: %f, %f----------", pos.x, pos.y);
	point_checker(pos, WIDTH, HEIGHT);
	//printf("XAFTER2: %f, %f\n", pos.x, pos.y);

	return pos;
}

vec2 find_index_from_map(vec2 pos) {
	//printf("before: %f, %f==========", pos.x, pos.y);

	//pos.x = pos.x * WIDTH + (window_width_px / (WIDTH * 2));
	//pos.y = pos.y * HEIGHT + (window_height_px / (HEIGHT * 2));

	pos.x = pos.x * (window_width_px / WIDTH);
	pos.y = pos.y * (window_height_px / HEIGHT);

	//printf("AFTER: %f, %f----------", pos.x, pos.y);
	point_checker(pos, window_width_px, window_height_px);
	//printf("AFTER2: %f, %f\n", pos.x, pos.y);

	return pos;
}

std::list<vec2> reconstruct_path(vec2 start, std::map<std::pair<float, float>, vec2> squares, std::list<vec2>& path) {
	vec2 pos = start;

	do {
		path.push_back(pos);
		pos = squares.at({ pos.x, pos.y });
	} while (pos != vec2(-1, -1));

	return path;
}

std::list<vec2> bfs_follow_iter(std::vector<std::vector<char>>& vec, vec2 start, std::list<vec2>& path) {
	std::list<vec2> buf;
	//keep track of previous square <current, parent>
	std::map<std::pair<float, float>, vec2> squares;
	buf.emplace_back(start);
	squares.emplace(std::make_pair(start.x, start.y), vec2(-1, -1));
	vec2 current;

	while (!buf.empty()) {
		current = buf.front();
		if (vec[current.x][current.y] == 'g') return reconstruct_path(current, squares, path);
		vec[current.x][current.y] = 'v';

		vec2 n;
		for (vec2 next_n : next) {
			n = current + next_n;

			if (n.x < vec.size() && n.x >= 0 && n.y < vec[0].size() && n.y >= 0 && vec[n.x][n.y] != 'b' && vec[n.x][n.y] != 'v') {
				buf.emplace_back(n);
				squares.emplace(std::make_pair(n.x, n.y), current);
			}
		}
		buf.pop_front();
	}

	return path;
}

void bfs_follow_start(std::vector<std::vector<char>>& vec, vec2 pos_chase, vec2 pos_prey, Entity& chaser) {
	std::list<vec2> path;
	pos_prey = find_map_index(pos_prey);
	pos_chase = find_map_index(pos_chase);

	//printf("START VEC %d %d AT %f %f\n", vec.size(), vec[0].size(), pos_chase.x, pos_chase.y);
	//printf("GOAL VEC %d %d AT %f %f\n", vec.size(), vec[0].size(), pos_prey.x, pos_prey.y);

	vec[pos_prey.x][pos_prey.y] = 'g';

	/*for (int i = 0; i < vec[0].size(); i++) {
		for (int j = 0; j < vec.size(); j++) {
			printf("%c", vec[j][i]);
		}
		printf("\n");
	} */

	registry.followingEnemies.get(chaser).path = bfs_follow_iter(vec, pos_chase, path);

	/*std::list<vec2> pth = astar_follow(vec, pos_chase, pos_chase);

	printf("HELLOOO: %d-------------------------------------------\n", pth.size());

	int i = pth.size();
	while (i > 0) {
		printf("Path %d: %f %f\n", i, pth.back().x, pth.back().y);
		pth.push_front(pth.back());
		pth.pop_back();
		i--;
	}*/

	//int i = registry.enemies.get(chaser).path.size();
	//while (i > 0) {
	//	printf("Path %d: %f %f\n", i, registry.enemies.get(chaser).path.back().x, registry.enemies.get(chaser).path.back().y);
	//	registry.enemies.get(chaser).path.push_front(registry.enemies.get(chaser).path.back());
	//	registry.enemies.get(chaser).path.pop_back();
	//	i--;
	//}
	//printf("GOODBYE: %d===========================================\n", registry.enemies.get(chaser).path.size());
}

//float get_distance(vec2 start, vec2 goal) {
//	return dot(start - goal, start - goal);
//}
//
//std::list<vec2> reconstruct_path_astar(Grid_square current, std::map<std::pair<float, float>, Grid_square> squares) {
//	std::list<vec2> path;
//	vec2 pos;
//
//	path.push_front(current.pos);
//	vec2 prev = current.pos;
//
//	while (1) {
//		pos = squares.find({ current.pos.x, current.pos.y })->second.came_from;
//		path.push_front(pos);
//
//		if (pos == prev) break;
//		prev = pos;
//	}
//	return path;
//}
//
//std::list<vec2> astar_follow(std::vector<std::vector<char>>& vec, vec2 start, vec2 goal) {
//	// The set of discovered nodes that may need to be (re-)expanded.
//	// Initially, only the start node is known.
//	// Implemented as a priority queue.
//	Grid_square new_square = Grid_square{ start, start, 0.f, 0.f + distance(start, goal) };
//
//	std::priority_queue<Grid_square, std::vector<Grid_square>, LessThanByF> open_set;
//	std::map<std::pair<float, float>, Grid_square> squares;
//	std::unordered_set<vec2> visited;
//
//	open_set.emplace(new_square);
//	squares.emplace(start, new_square);
//	visited.emplace(start);
//
//	while (!open_set.empty()) {
//		Grid_square current = open_set.top();
//		if (vec[current.pos.x][current.pos.y] == 'g') {
//			return reconstruct_path_astar(current, squares);
//		}
//
//		visited.erase(current.pos);
//		open_set.pop();
//		for (vec2 n: next) {
//			vec2 neighbor_pos = current.pos + n;
//
//			if (neighbor_pos.x >= vec.size() && neighbor_pos.x < 0 && neighbor_pos.y >= vec[0].size() && neighbor_pos.y < 0 && vec[neighbor_pos.x][neighbor_pos.y] == 'b') {
//				continue;
//			}
//
//			float temp_g_score = current.g + distance(current.pos, neighbor_pos);
//			if (squares.count({ neighbor_pos.x, neighbor_pos.y }) == 0 || temp_g_score < squares.find({ neighbor_pos.x, neighbor_pos.y })->second.g) {
//				Grid_square neighbor_square = Grid_square{ neighbor_pos, current.pos, temp_g_score, (temp_g_score + distance(neighbor_pos, goal)) };
//				squares.insert_or_assign({ neighbor_pos.x, neighbor_pos.y }, neighbor_square);
//
//				if (visited.count(neighbor_pos) > 0) {
//					open_set.emplace(neighbor_square);
//					visited.emplace(neighbor_pos);
//				}
//			
//			}
//
//		}
//	}
//
//	return reconstruct_path_astar(new_square, squares);
//}

//void bfs_follow_start(std::vector<std::vector<char>>& vec, vec2 pos_chase, vec2 pos_prey, Entity& chaser) {
//	std::list<vec2> path;
//	pos_prey = find_map_index(pos_prey);
//	pos_chase = find_map_index(pos_chase);
//
//	printf("ACCESSING VEC %d %d AT %d %d\n", vec.size(), vec[0].size(), (int)pos_prey.x, (int)pos_prey.y);
//
//	vec[pos_prey.x][pos_prey.y] = 'g';
//
//	for (int i = 0; i < vec.size(); i++) {
//		for (int j = 0; j < vec[0].size(); j++) {
//			printf("%c", vec[i][j]);
//		}
//		printf("\n");
//	}
//
//	registry.enemies.get(chaser).path = astar_follow(vec, pos_chase, pos_prey);
//
//	int i = 0;
//	while (!registry.enemies.get(chaser).path.empty()) {
//		i++;
//		printf("Path %d: %f %f\n", i, registry.enemies.get(chaser).path.back().x, registry.enemies.get(chaser).path.back().y);
//		registry.enemies.get(chaser).path.pop_back();
//	}
//	//move_step(chaser);
//}

// Creates a grid of WIDTH * HEIGHT, returned vector is vec[x][y]
// Grid values are n = nothing, b = block, v = visited, g = goal
std::vector<std::vector<char>> create_grid() {
	std::vector <char> line(HEIGHT, 'n');
	std::vector<std::vector<char> > vect(WIDTH, line);

	return vect;
}

void fill_grid(std::vector<std::vector<char>>& grid, vec2 pos, vec2 size) {
	vec2 point_start = pos - (size / 2.f);
	vec2 point_end = pos + (size / 2.f);

	//printf("++Point start real: %f, %f_____Point end real: %f, %f\n", point_start.x, point_start.y, point_end.x, point_end.y);

	point_start = find_map_index(point_start);
	point_end = find_map_index(point_end);

	//printf("Point start GRID: %f, %f_____Point end GRID: %f, %f\n", point_start.x, point_start.y, point_end.x, point_end.y);

	for (int i = point_start.x; i <= point_end.x; i++) {
		for (int j = point_start.y; j <= point_end.y; j++) {
			grid[i][j] = 'b';
		}
	}

}