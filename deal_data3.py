import numpy as np
import matplotlib.pyplot as plt
from matplotlib.gridspec import GridSpec
import matplotlib
# 设置字体为支持中文的字体
matplotlib.rcParams['font.sans-serif'] = ['SimHei'] # Windows 系统
# matplotlib.rcParams['font.sans-serif'] = ['Arial Unicode MS']  # macOS 系统
# matplotlib.rcParams['font.sans-serif'] = ['WenQuanYi Zen Hei']  # Linux 系统

# 设置正常显示负号
matplotlib.rcParams['axes.unicode_minus'] = False

# 定义常量
PEAKS_NUM_MAX = 5
PEAK_DIFF = 15
PEAK_FILL = 5

# 定义ABS宏的等效函数
def ABS(a, b):
    return a - b if a > b else b - a

# 峰参数类
class PeakDef:
    __slots__ = ('bulge_head', 'bulge_tail', 'head_index', 'tail_index', 
                 'refline', 'width', 'extreme_index', 'extreme_value',
                 'centroid_value', 'relative_altitude', 'centroid')
    
    def __init__(self):
        self.bulge_head = 0
        self.bulge_tail = 0
        self.head_index = 0
        self.tail_index = 0
        self.refline = 0
        self.width = 0
        self.extreme_index = 0
        self.extreme_value = 0
        self.centroid_value = 0
        self.relative_altitude = 0
        self.centroid = 0
        
    def __str__(self):
        return (f"Peak(head={self.bulge_head}, tail={self.bulge_tail}, "
                f"extreme={self.extreme_index}@{self.extreme_value:.2f}, "
                f"width={self.width})")

# 局部找峰函数 - 严格遵循原始C代码逻辑
def LocalHeadTailCalc(data, peak_count, thres, chirpRbCn, diff, peaks, start, end):
    """
    在数据中查找局部峰
    
    参数:
        data: 原始数据数组
        peak_count: 当前已找到的峰数量 (作为列表传递以便修改)
        thres: 阈值
        chirpRbCn: 数据总长度
        diff: 差分数据数组
        peaks: 峰对象列表
        start: 搜索起始索引
        end: 搜索结束索引
    """
    find_flag = False  # False: 找峰头, True: 找峰尾
    partial_sum = 0.0
    decline_flag = False
    extreme_value = 0.0
    current_peak_index = peak_count[0]
    
    i = start
    while i < end:
        # 如果已找到最大峰数，退出循环
        if current_peak_index >= PEAKS_NUM_MAX:
            break
        
        # 检查是否在查找峰尾
        if find_flag:
            # 情况1: 到达数据末尾
            if i == chirpRbCn - 2:
                peaks[current_peak_index].bulge_tail = i 
                current_peak_index += 1
                find_flag = False
                decline_flag = False
            
            # 情况2: 峰下降太多
            elif extreme_value > data[i] + 5 and i + 1 < len(data) and data[i + 1] > data[i]:
                peaks[current_peak_index].bulge_tail = i
                current_peak_index += 1
                find_flag = False
                decline_flag = False
            
            # 情况3: 主要峰尾检测条件
            elif (ABS(diff[i], 0) <= thres and 
                  (diff[i - 1] <= -thres or decline_flag) and 
                  data[i] < PEAK_FILL):
                
                partial_sum = 0.0
                decline_flag = True
                
                # 计算部分和 (原始C代码中的height替换为chirpRbCn)
                j_end = min(i + 4, chirpRbCn - 1)
                for j in range(i + 1, j_end):
                    if j < len(diff):  # 边界检查
                        partial_sum += diff[j]
                
                # 检查是否满足结束条件
                if (ABS(partial_sum, 0) < thres * 3 and ABS(data[i + 1], data[i]) < 3) or \
                   (i + 1 < len(data) and data[i + 1] < data[peaks[current_peak_index].bulge_head]):
                    
                    # 向后查找合适的结束点
                    j_val = i
                    # 注意: 原始C代码中j从i+1开始递减到i-3
                    found = False
                    for j in range(min(i + 1, chirpRbCn - 1), max(i - 4, -1) - 1, -1):
                        if j >= 2:  # 确保有j-2和j-1索引
                            if data[j] * 2 <= data[j - 2] + data[j - 1]:
                                j_val = j
                                found = True
                                break
                    
                    if not found:
                        j_val = i  # 如果没找到，使用当前i
                    
                    peaks[current_peak_index].bulge_tail = j_val
                    current_peak_index += 1
                    i = j_val - 1  # 更新索引位置 (注意-1，因为循环会i++)
                    find_flag = False
                    decline_flag = False
        
        # 查找峰头的条件
        if not find_flag:
            if i > 4 and i < chirpRbCn - 5:  # 边界检查
                if diff[i] >= thres:
                    # 创建新峰对象
                    if current_peak_index < len(peaks):
                        peaks[current_peak_index] = PeakDef()
                    else:
                        peaks.append(PeakDef())
                    
                    peaks[current_peak_index].bulge_head = i-1
                    find_flag = True
                    extreme_value = data[i]  # 重置极值为当前值
        
        # 更新极值 (仅在查找峰尾时)
        if find_flag and i < len(data):
            if data[i] >= extreme_value:
                extreme_value = data[i]
        
        i += 1
    
    # 更新峰计数
    peak_count[0] = current_peak_index

# 辅助函数：计算差分 (与之前相同)
def DiffCalc(data, offset_size, chirpRbCn):
    max_value_temp = data[0]
    max_value_index_temp = 0
    sum_val = 0.0
    diff = [0.0] * chirpRbCn  # 创建足够长的差分数组
    
    # 计算第一个差分值
    if offset_size + 1 < chirpRbCn:
        diff[0] = data[offset_size + 1] - data[0]
    
    # 计算最后一个差分值
    if chirpRbCn - offset_size - 1 >= 0:
        diff[chirpRbCn - offset_size] = data[chirpRbCn - 1] - data[chirpRbCn - offset_size - 1]
    
    # 计算中间差分值
    for i in range(1, chirpRbCn - offset_size):
        if i + offset_size < chirpRbCn:
            diff[i] = data[i + offset_size] - data[i - 1]
        
        # 更新最大值
        if data[i] > max_value_temp:
            max_value_index_temp = i
            max_value_temp = data[i]
        
        sum_val += data[i]
    
    # 计算平均值
    if chirpRbCn - offset_size - 1 > 0:
        average_value = sum_val / (chirpRbCn - offset_size - 1)
    else:
        average_value = 0.0
    
    return diff, max_value_temp, average_value, max_value_index_temp

# 辅助函数：可视化结果
def visualize_peak_detection(data, diff, peaks, peak_count):
    import matplotlib.pyplot as plt
    
    plt.figure(figsize=(14, 8))
    
    # 绘制原始数据
    plt.plot(data, 'b-', linewidth=1.5, label='原始数据')
    
    # 绘制差分数据
    plt.plot(diff, 'r-', alpha=0.6, linewidth=1, label='差分数据')
    
    # 标记检测到的峰
    for i in range(peak_count[0]):
        peak = peaks[i]
        plt.axvline(x=peak.bulge_head, color='g', linestyle='--', alpha=0.7, label='峰头' if i == 0 else "")
        plt.axvline(x=peak.bulge_tail, color='r', linestyle='--', alpha=0.7, label='峰尾' if i == 0 else "")
        
        # 标记峰区域
        plt.axvspan(peak.bulge_head, peak.bulge_tail, alpha=0.1, color='green', label='峰区域' if i == 0 else "")
        
        # 标记峰中心
        plt.plot(peak.bulge_head, data[peak.bulge_head], 'go', markersize=8)
        plt.plot(peak.bulge_tail, data[peak.bulge_tail], 'ro', markersize=8)
        
        # 添加标签
        plt.text(peak.bulge_head, data[peak.bulge_head] + 5, 
                 f'峰 {i+1} 头', fontsize=9, ha='center')
        plt.text(peak.bulge_tail, data[peak.bulge_tail] + 5, 
                 f'峰 {i+1} 尾', fontsize=9, ha='center')
    
    plt.title(f'峰检测结果 (检测到 {peak_count[0]} 个峰)')
    plt.xlabel('样本索引')
    plt.ylabel('幅值')
    plt.legend()
    plt.grid(True, alpha=0.3)
    plt.tight_layout()
    plt.show()

# 示例使用
if __name__ == "__main__":
    # 示例数据 (使用第一个Chirp的数据)
 #   data = [46.62, 49.82, 5.83, 6.40, 0.79, 1.95, 1.35, 1.00, 3.25, 6.50, 4.00, 2.06, 1.41, 0.50, 0.79, 0.56, 0.71, 1.46, 1.12, 1.41, 0.35, 1.50, 0.56, 0.90, 3.13, 7.16, 4.60, 2.70, 5.32, 3.20, 2.50, 3.05, 7.96, 6.75, 4.14, 6.36, 3.95, 15.20, 21.71, 9.66, 5.34, 9.51, 9.26, 2.37, 10.80, 8.00, 5.67, 5.94, 3.48, 4.78, 2.51, 2.37, 5.77, 3.16, 2.69, 2.02, 1.46, 1.60, 2.24, 0.56, 1.12, 1.52, 1.12, 1.27]
    data =[52.00, 55.72, 3.58, 33.23, 39.54, 10.06, 1.12, 4.26, 2.93, 2.85, 2.50, 3.29, 8.40, 7.62, 14.30, 16.97, 6.27, 4.32, 4.37, 2.57, 18.11, 52.26, 57.96, 22.01, 7.21, 8.16, 5.10, 8.04, 10.57, 10.31, 6.83, 2.06, 5.93, 4.81, 6.64, 6.05, 5.15, 5.64, 2.24, 3.29, 5.46, 5.27, 6.56, 5.02, 0.56, 3.95, 2.37, 2.02, 1.68, 1.77, 3.54, 5.27, 3.54, 0.25, 0.00, 0.90, 0.71, 1.68, 1.68, 2.26, 3.13, 5.94, 5.20, 2.85]
    chirpRbCn = len(data)
    offset_size = 1  # 差分偏移量
    thres = 2  # 峰检测阈值
    start_index = 5  # 起始搜索位置
    end_index = chirpRbCn - 5  # 结束搜索位置
    
    # 计算差分
    diff, max_val, avg_val, max_idx = DiffCalc(data, offset_size, chirpRbCn)
    
    # 初始化峰检测
    peak_count = [0]  # 使用列表模拟指针行为
    peaks = [PeakDef() for _ in range(PEAKS_NUM_MAX)]  # 预创建峰对象
    
    # 执行峰检测
    LocalHeadTailCalc(data, peak_count, thres, chirpRbCn, diff, peaks, start_index, end_index)
    
    # 打印结果
    print(f"检测到 {peak_count[0]} 个峰:")
    for i in range(peak_count[0]):
        print(f"峰 {i+1}:")
        print(f"  头位置: {peaks[i].bulge_head} (值: {data[peaks[i].bulge_head]:.2f})")
        print(f"  尾位置: {peaks[i].bulge_tail} (值: {data[peaks[i].bulge_tail]:.2f})")
        print(f"  宽度: {peaks[i].bulge_tail - peaks[i].bulge_head} 个样本")
    
    # 可视化结果
    visualize_peak_detection(data, diff, peaks, peak_count)