void setup() {
  Serial.begin(9600);
}

void loop() {
  int t = 2;
  int n = 1;
  while (n < 100) {
    float x[n];
    float noise[n];
    for (int i = 0; i < n; i++) {
      x[i] = float(i + 1);
      noise[i] = float(random(-2 * n, 2 * n));
    }
    float ytrue[n];
    float y[n];
    for (int i = 0; i < n; i++) {
      ytrue[i] = -x[i] * x[i];
      y[i] = -x[i] * x[i] + noise[i];
    }
    float coefficients[3];
    polyfit(x, y, n, coefficients);
    float x1[n];
    float y1[n];
    for (int i = 0; i < n; i++) {
      x1[i] = float(i + 1);
      y1[i] = coefficients[0] * x1[i] * x1[i] + coefficients[1] * x1[i] + coefficients[2];
    }
    int max_y1_index = findMaxIndex(y1, n);
    int max_ytrue_index = findMaxIndex(ytrue, n);
    int error = max_y1_index - max_ytrue_index;
    plot(x, y, n);
    plot(x1, y1, n);
    plot(x1, ytrue, n);
    n++;
  }
}

void polyfit(float x[], float y[], int n, float coefficients[]) {
  float sum_x = 0;
  float sum_x2 = 0;
  float sum_x3 = 0;
  float sum_x4 = 0;
  float sum_y = 0;
  float sum_xy = 0;
  float sum_x2y = 0;
  for (int i = 0; i < n; i++) {
    float xi = x[i];
    float xi2 = xi * xi;
    float xi3 = xi2 * xi;
    float xi4 = xi3 * xi;
    float yi = y[i];
    sum_x += xi;
    sum_x2 += xi2;
    sum_x3 += xi3;
    sum_x4 += xi4;
    sum_y += yi;
    sum_xy += xi * yi;
    sum_x2y += xi2 * yi;
  }
  float denom = (n * sum_x4 - sum_x2 * sum_x2);
  coefficients[0] = (n * sum_x2y - sum_xy * sum_x2) / denom;
  coefficients[1] = (sum_xy * n - sum_x * sum_xy) / denom;
  coefficients[2] = (sum_y - coefficients[0] * sum_x2 - coefficients[1] * sum_x) / n;
}

int findMaxIndex(float array[], int n) {
  int max_index = 0;
  float max_value = array[0];
  for (int i = 1; i < n; i++) {
    if (array[i] > max_value) {
      max_value = array[i];
      max_index = i;
    }
  }
  return max_index;
}

void plot(float x[], float y[], int n) {
  for (int i = 0; i < n; i++) {
    Serial.print(x[i]);
    Serial.print(",");
    Serial.print(y[i]);
    Serial.print("o");
    Serial.print(" ");