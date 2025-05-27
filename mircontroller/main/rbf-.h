#ifndef RBF-_H
#define RBF-_H
// === PHẦN MẠNG RBF ===

// Số lượng nơ-ron trong lớp ẩn của mạng RBF
const int NUM_NEURONS_RBF = 3;

// Mảng chứa các giá trị tâm (mu) của các hàm Gaussian cho mỗi nơ-ron.
// Các tâm này tương ứng với các giá trị sai số (e) cụ thể.
double rbf_mu[NUM_NEURONS_RBF] = {0, 20, 100};

// Mảng chứa các giá trị độ rộng (sigma) của các hàm Gaussian cho mỗi nơ-ron.
// Sigma xác định mức độ "lan tỏa" của hàm Gaussian.
double rbf_sigma[NUM_NEURONS_RBF] = {10, 15, 25};

// Ma trận trọng số của mạng RBF.
// weights[i][0] là trọng số từ nơ-ron i đến Kp.
// weights[i][1] là trọng số từ nơ-ron i đến Ki.
// weights[i][2] là trọng số từ nơ-ron i đến Kd.
double rbf_weights[NUM_NEURONS_RBF][3];

// Các biến lưu trữ giá trị Kp, Ki, Kd được tính toán bởi mạng RBF
double Kp_rbf, Ki_rbf, Kd_rbf;

void initializeWeights();
double gaussian(double x, double mu, double sigma);
double computePhi(int i, double e_val, double sp_val);
void computePIDRBF(double e_val, double sp_val);
void updateWeights(double e_val, double sp_val);

#endif