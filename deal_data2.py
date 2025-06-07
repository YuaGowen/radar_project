import matplotlib.pyplot as plt
import numpy as np

def DiffCalc(data, offset_size, chirpRbCn):
    """
    计算数据数组的差分、最大值、平均值和最大值索引
    并返回原始数据、差分数据及计算结果
    
    参数:
        data: 输入数据列表或数组
        offset_size: 差分计算偏移量
        chirpRbCn: 数据长度
    
    返回:
        diff: 差分结果数组
        max_value: 数据最大值
        average_value: 数据平均值
        max_value_index: 最大值索引
    """
    max_value_temp = data[0]
    max_value_index_temp = 0
    sum_val = 0.0
    
    # 初始化差分数组
    diff = [0.0] * (chirpRbCn - offset_size + 1)
    
    # 计算第一个差分值
    if offset_size + 1 < chirpRbCn:
        diff[0] = data[offset_size + 1] - data[0]
    else:
        diff[0] = 0.0
    
    # 计算最后一个差分值
    if chirpRbCn - offset_size - 1 >= 0:
        diff[chirpRbCn - offset_size] = data[chirpRbCn - 1] - data[chirpRbCn - offset_size - 1]
    else:
        diff[chirpRbCn - offset_size] = 0.0
    
    # 计算中间差分值并统计最大值和平均值
    for i in range(1, chirpRbCn - offset_size):
        # 确保索引在有效范围内
        if i + offset_size < chirpRbCn:
            diff[i] = data[i + offset_size] - data[i - 1]
        else:
            diff[i] = 0.0
        
        # 更新最大值
        if data[i] > max_value_temp:
            max_value_index_temp = i
            max_value_temp = data[i]
        
        # 累加用于平均值计算
        sum_val += data[i]
    
    # 计算平均值
    if chirpRbCn - offset_size - 1 > 0:
        average_value = sum_val / (chirpRbCn - offset_size - 1)
    else:
        average_value = 0.0
    
    return diff, max_value_temp, average_value, max_value_index_temp

def plot_data_and_diff(data, offset_size, chirpRbCn, chirp_id=0):
    """
    计算并绘制原始数据和差分数据
    
    参数:
        data: 原始数据
        offset_size: 差分偏移量
        chirpRbCn: 数据长度
        chirp_id: 用于标题的Chirp ID
    """
    # 计算差分
    diff, max_val, avg_val, max_idx = DiffCalc(data, offset_size, chirpRbCn)
    
    # 创建图表
    plt.figure(figsize=(14, 8))
    
    # 原始数据曲线
    plt.subplot(2, 1, 1)
    plt.plot(data, 'b-o', markersize=4, linewidth=1.5, label='原始数据')
    plt.plot(max_idx, max_val, 'ro', markersize=8, label=f'最大值: {max_val:.2f} @ {max_idx}')
    plt.axhline(y=avg_val, color='g', linestyle='--', label=f'平均值: {avg_val:.2f}')
    plt.title(f'Chirp {chirp_id} - 原始数据 (长度: {chirpRbCn})')
    plt.xlabel('样本索引')
    plt.ylabel('幅值')
    plt.legend()
    plt.grid(True, alpha=0.3)
    
    # 差分数据曲线
    plt.subplot(2, 1, 2)
    plt.plot(diff, 'r-o', markersize=4, linewidth=1.5, label='差分数据')
    plt.axhline(y=0, color='k', linestyle='-', alpha=0.3)
    plt.title(f'差分数据 (偏移量: {offset_size})')
    plt.xlabel('样本索引')
    plt.ylabel('差分值')
    plt.legend()
    plt.grid(True, alpha=0.3)
    
    plt.tight_layout()
    plt.show()
    
    # 返回计算结果
    return diff, max_val, avg_val, max_idx

# 使用示例 - 第一个Chirp的数据
data = [46.62, 49.82, 5.83, 6.40, 0.79, 1.95, 1.35, 1.00, 3.25, 6.50, 4.00, 2.06, 1.41, 0.50, 0.79, 0.56, 0.71, 1.46, 1.12, 1.41, 0.35, 1.50, 0.56, 0.90, 3.13, 7.16, 4.60, 2.70, 5.32, 3.20, 2.50, 3.05, 7.96, 6.75, 4.14, 6.36, 3.95, 15.20, 21.71, 9.66, 5.34, 9.51, 9.26, 2.37, 10.80, 8.00, 5.67, 5.94, 3.48, 4.78, 2.51, 2.37, 5.77, 3.16, 2.69, 2.02, 1.46, 1.60, 2.24, 0.56, 1.12, 1.52, 1.12, 1.27]

# 设置参数
offset_size = 1  # 差分偏移量
chirpRbCn = len(data)  # 数据长度

# 计算并绘图
diff, max_val, avg_val, max_idx = plot_data_and_diff(data, offset_size, chirpRbCn, chirp_id=0)

# 打印结果
print("计算结果:")
print(f"最大值: {max_val:.2f} 在索引 {max_idx}")
print(f"平均值: {avg_val:.2f}")
print(f"差分数组长度: {len(diff)}")
print(f"前5个差分值: {diff[:5]}")
print(f"最后5个差分值: {diff[-5:]}")


