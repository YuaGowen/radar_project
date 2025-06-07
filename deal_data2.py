import matplotlib.pyplot as plt
import numpy as np

def DiffCalc(data, offset_size, chirpRbCn):
    """
    ������������Ĳ�֡����ֵ��ƽ��ֵ�����ֵ����
    ������ԭʼ���ݡ�������ݼ�������
    
    ����:
        data: ���������б������
        offset_size: ��ּ���ƫ����
        chirpRbCn: ���ݳ���
    
    ����:
        diff: ��ֽ������
        max_value: �������ֵ
        average_value: ����ƽ��ֵ
        max_value_index: ���ֵ����
    """
    max_value_temp = data[0]
    max_value_index_temp = 0
    sum_val = 0.0
    
    # ��ʼ���������
    diff = [0.0] * (chirpRbCn - offset_size + 1)
    
    # �����һ�����ֵ
    if offset_size + 1 < chirpRbCn:
        diff[0] = data[offset_size + 1] - data[0]
    else:
        diff[0] = 0.0
    
    # �������һ�����ֵ
    if chirpRbCn - offset_size - 1 >= 0:
        diff[chirpRbCn - offset_size] = data[chirpRbCn - 1] - data[chirpRbCn - offset_size - 1]
    else:
        diff[chirpRbCn - offset_size] = 0.0
    
    # �����м���ֵ��ͳ�����ֵ��ƽ��ֵ
    for i in range(1, chirpRbCn - offset_size):
        # ȷ����������Ч��Χ��
        if i + offset_size < chirpRbCn:
            diff[i] = data[i + offset_size] - data[i - 1]
        else:
            diff[i] = 0.0
        
        # �������ֵ
        if data[i] > max_value_temp:
            max_value_index_temp = i
            max_value_temp = data[i]
        
        # �ۼ�����ƽ��ֵ����
        sum_val += data[i]
    
    # ����ƽ��ֵ
    if chirpRbCn - offset_size - 1 > 0:
        average_value = sum_val / (chirpRbCn - offset_size - 1)
    else:
        average_value = 0.0
    
    return diff, max_value_temp, average_value, max_value_index_temp

def plot_data_and_diff(data, offset_size, chirpRbCn, chirp_id=0):
    """
    ���㲢����ԭʼ���ݺͲ������
    
    ����:
        data: ԭʼ����
        offset_size: ���ƫ����
        chirpRbCn: ���ݳ���
        chirp_id: ���ڱ����Chirp ID
    """
    # ������
    diff, max_val, avg_val, max_idx = DiffCalc(data, offset_size, chirpRbCn)
    
    # ����ͼ��
    plt.figure(figsize=(14, 8))
    
    # ԭʼ��������
    plt.subplot(2, 1, 1)
    plt.plot(data, 'b-o', markersize=4, linewidth=1.5, label='ԭʼ����')
    plt.plot(max_idx, max_val, 'ro', markersize=8, label=f'���ֵ: {max_val:.2f} @ {max_idx}')
    plt.axhline(y=avg_val, color='g', linestyle='--', label=f'ƽ��ֵ: {avg_val:.2f}')
    plt.title(f'Chirp {chirp_id} - ԭʼ���� (����: {chirpRbCn})')
    plt.xlabel('��������')
    plt.ylabel('��ֵ')
    plt.legend()
    plt.grid(True, alpha=0.3)
    
    # �����������
    plt.subplot(2, 1, 2)
    plt.plot(diff, 'r-o', markersize=4, linewidth=1.5, label='�������')
    plt.axhline(y=0, color='k', linestyle='-', alpha=0.3)
    plt.title(f'������� (ƫ����: {offset_size})')
    plt.xlabel('��������')
    plt.ylabel('���ֵ')
    plt.legend()
    plt.grid(True, alpha=0.3)
    
    plt.tight_layout()
    plt.show()
    
    # ���ؼ�����
    return diff, max_val, avg_val, max_idx

# ʹ��ʾ�� - ��һ��Chirp������
data = [46.62, 49.82, 5.83, 6.40, 0.79, 1.95, 1.35, 1.00, 3.25, 6.50, 4.00, 2.06, 1.41, 0.50, 0.79, 0.56, 0.71, 1.46, 1.12, 1.41, 0.35, 1.50, 0.56, 0.90, 3.13, 7.16, 4.60, 2.70, 5.32, 3.20, 2.50, 3.05, 7.96, 6.75, 4.14, 6.36, 3.95, 15.20, 21.71, 9.66, 5.34, 9.51, 9.26, 2.37, 10.80, 8.00, 5.67, 5.94, 3.48, 4.78, 2.51, 2.37, 5.77, 3.16, 2.69, 2.02, 1.46, 1.60, 2.24, 0.56, 1.12, 1.52, 1.12, 1.27]

# ���ò���
offset_size = 1  # ���ƫ����
chirpRbCn = len(data)  # ���ݳ���

# ���㲢��ͼ
diff, max_val, avg_val, max_idx = plot_data_and_diff(data, offset_size, chirpRbCn, chirp_id=0)

# ��ӡ���
print("������:")
print(f"���ֵ: {max_val:.2f} ������ {max_idx}")
print(f"ƽ��ֵ: {avg_val:.2f}")
print(f"������鳤��: {len(diff)}")
print(f"ǰ5�����ֵ: {diff[:5]}")
print(f"���5�����ֵ: {diff[-5:]}")


