#include <iostream>
#include <fstream>
#include <functional>
#include <vector>
#include <ctime>

double line(double x) {
	double k = 0.005;
	double b = -2;
	return k * x + b;
};

double parabola(double x) {
	long double a = 0.00001;
	long double b = 0.0001;
	long double c = 2000;
	return a * long double(x) * long double(x) + b * long double(x) + c;
}

struct DATA {
	double x;
	double y;

	friend std::ostream& operator<<(std::ostream& os, const DATA& data);
	friend std::istream& operator>>(std::istream& is, DATA& data);

	DATA() {
		this->x = 0.0;
		this->y = 0.0;
	}
	DATA(double x, double y) {
		this->x = x;
		this->y = y;
	}
};

std::ostream& operator<<(std::ostream& os, const DATA& data) {
	os << data.x << ";" << data.y << "\n";
	return os;
}
std::istream& operator>>(std::istream& is, DATA& data) {
	char buffer_char;
	is >> data.x >> buffer_char >> data.y;
	return is;
}

class Generator {
	typedef std::function<double(double x)> bias_function;
	typedef std::vector<bias_function> bias_vector;

	DATA point;

	size_t lenght;
	size_t deep;

	bias_vector biasis;

	double** W;
	double** U;
	double** V;
	double** Y;

	void create_weight() {
		this->W = new double* [this->lenght];
		this->V = new double* [this->lenght];
		this->U = new double* [this->lenght];
		this->Y = new double* [this->lenght];
		for (size_t i = 0; i < this->lenght; i++) {
			this->W[i] = new double[this->deep];
			this->V[i] = new double[this->deep];
			this->U[i] = new double[this->deep];
			this->Y[i] = new double[this->deep];
		}
	}
	void randfill_weight(int min, int max, int tiff) {
		for (size_t i = 0; i < this->lenght; i++) {
			for (size_t j = 0; j < this->deep; j++) {
				this->W[i][j] = ((rand() % (max + 1)) - min) / tiff;
				this->U[i][j] = ((rand() % (max + 1)) - min) / tiff;
				this->Y[i][j] = ((rand() % (max + 1)) - min) / tiff;
				this->V[i][j] = ((rand() % (max + 1)) - min) / tiff;
			}
		}
	}
	void delete_weight() {
		for (size_t i = 0; i < this->lenght; i++) {
			delete[] this->W[i];
			delete[] this->U[i];
			delete[] this->V[i];
			delete[] this->Y[i];
		}
		delete[] this->W;
		delete[] this->U;
		delete[] this->V;
		delete[] this->Y;
	}

public:
	DATA* generate(size_t quantity, double x_start = 0.0, double x_step = 1.0) {
		create_weight();

		DATA* answer = new DATA[quantity];

		for (size_t k = 0; k < quantity; k++) {
			randfill_weight(-1000, 1000, 1000);
			double* temp = new double[this->lenght]{0};
			double y = 0;
			for (size_t i = 0; i < this->lenght; i++) {
				for (size_t j = 0; j < this->deep; j++) {
					temp[i] += this->W[i][j] * ((rand() % (101)) - 0) / 100;
					temp[i] += this->U[i][j] * ((rand() % (101)) - 0) / 100;
					temp[i] += this->Y[i][j] * ((rand() % (101)) - 0) / 100;
					temp[i] += this->V[i][j] * ((rand() % (101)) - 0) / 100;
				}
				y += temp[i];
			}

			for (size_t i = 0; i < biasis.size(); i++) {
				y += biasis[i](x_start + x_step * k);
			}

			answer[k].x = x_start + x_step * k;
			answer[k].y = y;

			delete[] temp;
		}

		delete_weight();

		return answer;
	}

	void push_bias(bias_function b_func) {
		biasis.push_back(b_func);
	}
	void pop_bias() {
		biasis.pop_back();
	}

	Generator(size_t lenght, size_t deep) {
		this->lenght = lenght;
		this->deep = deep;
	}
};

void dump_data(DATA*& data, size_t size, std::string file_name) {
	std::ofstream result_file;
	result_file.open(file_name);
	for (int i = 0; i < size; i++)
		result_file << data[i];
	result_file.close();
}

int main()
{
    setlocale(LC_ALL, "Russian");
    srand(time(NULL));

	std::cout << "Начало\n";

	Generator gen(12, 12);
	gen.push_bias(line);
	gen.push_bias(parabola);

	size_t size = 10000;
	DATA* data;
	data = gen.generate(size, 0, 2);
	std::cout << "Генерация завершена\n";

	dump_data(data, size, "data.csv");
	std::cout << "Загрузка данных завершена\n";

	system("python vis.py");
	return 0;
}
