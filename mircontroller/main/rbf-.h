#ifndef RBF-_H
#define RBF-_H
void initializeWeights();
double gaussian(double x, double mu, double sigma);
double computePhi(int i, double e_val, double sp_val);
void computePIDRBF(double e_val, double sp_val);
void updateWeights(double e_val, double sp_val);

#endif