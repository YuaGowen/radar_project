import matplotlib.pyplot as plt # type: ignore

#对窗外数据
# 定义 Magnitudes 和 Magnitudes2
Magnitudes = [416.00, 14.00, 15.26, 11.70, 404.00, 14.04, 16.12, 13.89, 391.00, 18.11, 16.55, 15.23, 387.00, 14.32, 16.55, 13.42, 386.00, 13.04, 14.42, 13.89, 379.00, 17.03, 14.42, 13.42, 380.00, 12.37, 16.64, 16.55, 376.00, 15.52, 14.87, 17.00, 374.00, 14.04, 12.81, 16.40, 373.00, 14.32, 16.12, 14.87, 366.00, 13.04, 17.89, 15.23, 378.00, 12.04, 17.49, 14.76, 373.00, 13.04, 17.46, 16.12, 367.00, 13.34, 14.87, 16.16, 375.00, 17.03, 15.65, 16.12, 374.00, 13.15, 16.12, 13.00]
Magnitudes2 =[-70.27, -58.71, -56.22, -58.62, -63.43, -63.34, -60.46, -50.12, -71.45, -57.39, -61.45, -67.67, -53.10, -50.96, -59.54, -67.84, -64.62, -64.95, -69.43, -69.50, -64.69, -58.82, -60.58, -72.19, -65.15, -70.13, -68.81, -61.75, -63.56, -65.24, -69.45, -65.34, -69.56, -68.21, -66.92, -61.20, -62.65, -65.59, -59.74, -49.76, -43.46, -53.84, -44.50, -41.62, -50.92, -65.37, -70.35, -63.69, -60.78, -68.05, -72.43, -64.32, -65.11, -70.15, -56.81, -51.32, -55.11, -61.57, -56.73, -60.96, -54.79, -65.52, -65.63, -65.79
]
Magnitudes3 = [-28.56, -22.57, -22.65, -30.46, -34.68, -40.13, -53.21, -49.37, -66.82, -58.84, -65.62, -69.08, -54.14, -52.57, -59.32, -62.94, -69.60, -65.73, -62.73, -69.00, -68.00, -63.08, -64.33, -69.30, -65.20, -65.04, -64.68, -59.59, -54.54, -61.94, -75.18, -71.73, -65.44, -80.77, -66.64, -64.85, -62.75, -70.66, -69.08, -48.59, -42.62, -52.25, -46.82, -42.57, -50.70, -57.49, -62.01, -62.07, -63.48, -66.37, -69.74, -76.22, -66.74, -64.07, -61.48, -50.87, -52.18, -86.22, -69.45, -62.31, -57.10, -55.79, -57.45, -65.96]
Magnitudes4 = [-28.61, -22.64, -22.55, -30.11, -35.25, -40.53, -56.61, -49.12, -68.33, -58.94, -66.77, -58.93, -52.93, -54.50, -64.92, -62.29, -61.33, -69.36, -75.67, -70.91, -63.43, -59.75, -70.64, -65.39, -61.57, -63.06, -67.25, -62.79, -64.78, -74.33, -74.80, -66.24, -68.43, -75.11, -68.87, -74.50, -61.18, -61.81, -64.20, -51.14, -43.44, -52.44, -47.67, -42.96, -50.86, -60.09, -63.93, -66.87, -60.87, -83.32, -60.18, -60.33, -64.06, -68.09, -59.22, -52.46, -53.64, -56.22, -56.84, -64.87, -51.76, -53.20, -56.95, -62.18]

Magnitudes5 =  [52.25, 56.89, 8.00, 8.31, 5.70, 5.81, 9.37, 21.22, 13.19, 18.74, 23.63, 46.00, 41.98, 11.77, 5.53, 10.06, 8.75, 3.50, 2.66, 5.62, 5.41, 3.36, 1.95, 20.13, 29.59, 11.63, 1.35, 2.76, 2.69, 2.24, 11.03, 17.27, 10.26, 5.13, 10.75, 14.20, 5.86, 4.07, 9.52, 17.85, 15.04, 12.39, 5.88, 4.42, 6.75, 4.92, 2.02, 3.58, 6.75, 4.07, 2.47, 4.78, 3.09, 1.50, 1.95, 2.00, 2.25, 1.52, 2.80, 1.82, 2.50, 1.52, 1.03, 1.95]

Magnitudes6 = [62.25, 61.95, 0.25, 8.76, 11.63, 7.72, 10.09, 24.49, 14.02, 10.18, 10.57, 49.39, 54.58, 16.28, 6.25, 4.78, 4.59, 2.15, 1.77, 2.47, 2.26, 3.20, 0.25, 15.03, 23.57, 9.92, 1.35, 1.68, 3.16, 8.38, 15.40, 22.06, 13.53, 8.20, 12.77, 19.95, 12.56, 3.36, 12.09, 22.08, 16.62, 11.69, 17.33, 12.02, 6.26, 2.30, 2.24, 2.66, 5.80, 6.02, 3.01, 1.27, 1.77, 4.30, 4.25, 3.58, 3.05, 2.80, 2.76, 1.35, 0.35, 1.27, 2.24, 1.90]
x = list(range(len(Magnitudes6)))

# 绘制曲线
plt.figure(figsize=(12, 6))
#plt.plot(x, Magnitudes, label='Magnitudes', marker='o')
# plt.plot(x, Magnitudes2, label='Magnitudes2', marker='x')
# plt.plot(x, Magnitudes3, label='Magnitudes3', marker='.')
# plt.plot(x, Magnitudes4, label='Magnitudes4', marker='*')
plt.plot(x, Magnitudes5, label='Magnitudes5', marker='+')
plt.plot(x, Magnitudes6, label='Magnitudes6', marker='s')
# 添加标题和标签
plt.title('Magnitude Comparison')
plt.xlabel('Range Bin Index')
plt.ylabel('Magnitude (dB)')
plt.legend()
plt.grid(True)

# 显示图表
plt.show()