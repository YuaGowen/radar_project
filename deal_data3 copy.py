import numpy as np
import matplotlib.pyplot as plt
from mpl_toolkits.mplot3d import Axes3D

# 雷达系统参数设置
class RadarParameters:
    def __init__(self):
        # 基本参数
        self.fc = 77e9          # 中心频率 (Hz)
        self.B = 4e9            # 带宽 (Hz)
        self.T = 40e-6          # 脉冲持续时间 (s)
        self.fs = 2e6           # 采样频率 (Hz)
        self.c = 3e8            # 光速 (m/s)
        
        # 帧结构参数
        self.chirps_per_frame = 128  # 每帧chirp数量
        self.samples_per_chirp = 256 # 每个chirp采样点数
        self.frames_per_second = 10  # 帧率 (Hz)
        
        # 天线配置
        self.num_tx_antennas = 3    # 发射天线数
        self.num_rx_antennas = 4    # 接收天线数
        self.antenna_spacing = 0.5  # 天线间距 (波长)
        
        # 计算衍生参数
        self.lambda_ = self.c / self.fc  # 波长 (m)
        self.freq_slope = self.B / self.T  # 频率斜率 (Hz/s)
        self.range_resolution = self.c / (2 * self.B)  # 距离分辨率 (m)
        self.max_range = self.c * self.fs / (2 * self.freq_slope)  # 最大可测距离 (m)
        self.velocity_resolution = self.lambda_ / (2 * self.T * self.chirps_per_frame)  # 速度分辨率 (m/s)
        self.max_velocity = self.lambda_ / (4 * self.T)  # 最大可测速度 (m/s)

# 生成模拟目标
def generate_targets():
    targets = [
        {"range": 50, "velocity": 10, "angle": 15},  # 目标1
        {"range": 80, "velocity": -5, "angle": -10},  # 目标2
        {"range": 120, "velocity": 0, "angle": 20}    # 目标3
    ]
    return targets

# 生成雷达回波信号
def generate_radar_echo(params, targets):
    # 初始化回波信号矩阵 [接收天线, 采样点, chirp]
    echo_signal = np.zeros((params.num_rx_antennas, 
                           params.samples_per_chirp, 
                           params.chirps_per_frame), dtype=complex)
    
    # 时间轴
    t_chirp = np.arange(params.samples_per_chirp) / params.fs  # 单个chirp内的时间
    t_frame = np.arange(params.chirps_per_frame) * params.T    # 帧内的chirp时间
    
    # 为每个目标生成回波
    for target in targets:
        # 计算目标参数
        tau = 2 * target["range"] / params.c  # 时延
        phase_shift = 4 * np.pi * target["range"] / params.lambda_  # 相位偏移
        doppler_shift = 2 * target["velocity"] / params.lambda_  # 多普勒频移
        
        # 计算每个接收天线的相位差（角度信息）
        for rx_idx in range(params.num_rx_antennas):
            # 基于阵列信号处理的相位差模型
            angle_rad = np.deg2rad(target["angle"])
            antenna_pos = rx_idx * params.antenna_spacing * params.lambda_
            phase_diff = 2 * np.pi * antenna_pos * np.sin(angle_rad) / params.lambda_
            
            # 生成回波信号
            for chirp_idx in range(params.chirps_per_frame):
                # 考虑多普勒效应的相位变化
                doppler_phase = 2 * np.pi * doppler_shift * t_frame[chirp_idx]
                
                # 生成带有时延和相位差的chirp信号
                delayed_chirp = np.exp(1j * (2 * np.pi * params.fc * (t_chirp - tau) + 
                                          np.pi * params.freq_slope * (t_chirp - tau)**2 + 
                                          phase_shift + phase_diff + doppler_phase))
                
                # 添加到回波信号矩阵（考虑目标RCS和噪声）
                snr_db = 20  # 信噪比 (dB)
                snr_linear = 10**(snr_db / 10)
                noise_power = 1 / snr_linear
                noise = np.random.normal(0, np.sqrt(noise_power/2), params.samples_per_chirp) + \
                       1j * np.random.normal(0, np.sqrt(noise_power/2), params.samples_per_chirp)
                
                echo_signal[rx_idx, :, chirp_idx] += 0.1 * delayed_chirp + noise  # RCS = 0.1
    
    return echo_signal

# 距离FFT（快时间处理）
def range_fft(echo_signal):
    # 对每个接收天线和每个chirp的采样点进行FFT
    range_data = np.fft.fft(echo_signal, axis=1)
    return range_data

# 速度FFT（慢时间处理）
def doppler_fft(range_data):
    # 对每个接收天线和每个距离bin的chirp进行FFT
    doppler_data = np.fft.fftshift(np.fft.fft(range_data, axis=2), axes=2)
    return doppler_data

# 角度估计 (MUSIC算法)
def estimate_angle(rd_matrix, params, target_range_bin, target_doppler_bin):
    # 提取特定距离-多普勒bin处的所有接收天线信号
    antenna_signals = rd_matrix[:, target_range_bin, target_doppler_bin].reshape(-1, 1)
    
    # 构建协方差矩阵
    R = antenna_signals @ antenna_signals.conj().T / len(antenna_signals)
    
    # 特征值分解
    eigenvalues, eigenvectors = np.linalg.eig(R)
    
    # 按特征值降序排序
    idx = eigenvalues.argsort()[::-1]
    eigenvalues = eigenvalues[idx]
    eigenvectors = eigenvectors[:, idx]
    
    # 估计信号子空间维度（假设有1个目标）
    signal_dim = 1
    noise_subspace = eigenvectors[:, signal_dim:]
    
    # 角度扫描
    angles = np.linspace(-90, 90, 361)
    P_MUSIC = np.zeros(len(angles), dtype=complex)
    
    # 构建阵列流形矩阵
    for i, theta in enumerate(angles):
        a = np.exp(1j * 2 * np.pi * params.antenna_spacing * 
                  np.arange(params.num_rx_antennas) * np.sin(np.deg2rad(theta)))
        a = a.reshape(-1, 1)
        
        # MUSIC谱
        P_MUSIC[i] = 1 / (a.conj().T @ noise_subspace @ noise_subspace.conj().T @ a)
    
    # 归一化并取模
    P_MUSIC = 10 * np.log10(np.abs(P_MUSIC) / np.max(np.abs(P_MUSIC)))
    
    # 找到最大谱峰对应的角度
    estimated_angle = angles[np.argmax(P_MUSIC)]
    
    return estimated_angle, angles, P_MUSIC

# 主函数
def main():
    # 初始化参数
    params = RadarParameters()
    
    # 生成模拟目标
    targets = generate_targets()
    
    # 生成雷达回波信号
    echo_signal = generate_radar_echo(params, targets)
    
    # 距离FFT处理
    range_data = range_fft(echo_signal)
    
    # 速度FFT处理
    rd_matrix = doppler_fft(range_data)
    
    # 计算距离和速度轴
    range_axis = np.arange(params.samples_per_chirp) * params.range_resolution
    velocity_axis = np.linspace(-params.max_velocity, params.max_velocity, params.chirps_per_frame)
    
    # 可视化距离-多普勒图
    plt.figure(figsize=(12, 8))
    plt.imshow(20 * np.log10(np.abs(rd_matrix[0, :, :]) + 1e-6), 
               aspect='auto', 
               extent=[velocity_axis[0], velocity_axis[-1], range_axis[-1], range_axis[0]],
               cmap='jet')
    plt.colorbar(label='幅度 (dB)')
    plt.xlabel('速度 (m/s)')
    plt.ylabel('距离 (m)')
    plt.title('距离-多普勒图 (RDM)')
    
    # 目标检测与角度估计
    detected_targets = []
    
    # 简单阈值检测
    threshold = np.max(20 * np.log10(np.abs(rd_matrix[0, :, :]))) - 25
    
    for r in range(10, params.samples_per_chirp - 10):  # 忽略边缘区域
        for v in range(10, params.chirps_per_frame - 10):
            if 20 * np.log10(np.abs(rd_matrix[0, r, v])) > threshold:
                # 确认是否为局部最大值
                if (np.abs(rd_matrix[0, r, v]) > np.abs(rd_matrix[0, r-1:r+2, v-1:v+2])).all():
                    # 估计角度
                    angle, angles, P_MUSIC = estimate_angle(rd_matrix, params, r, v)
                    
                    # 计算实际距离和速度
                    actual_range = r * params.range_resolution
                    actual_velocity = v * params.velocity_resolution - params.max_velocity
                    
                    detected_targets.append({
                        'range': actual_range,
                        'velocity': actual_velocity,
                        'angle': angle
                    })
                    
                    # 在RDM上标记目标
                    plt.plot(actual_velocity, actual_range, 'ro', markersize=8)
                    plt.text(actual_velocity, actual_range, f'{angle:.1f}°', color='white')
    
    # 打印检测结果
    print("检测到的目标:")
    for i, target in enumerate(detected_targets):
        print(f"目标 {i+1}: 距离 = {target['range']:.2f}m, 速度 = {target['velocity']:.2f}m/s, 角度 = {target['angle']:.2f}°")
    
    # 3D点云可视化
    fig = plt.figure(figsize=(12, 8))
    ax = fig.add_subplot(111, projection='3d')
    
    for target in detected_targets:
        x = target['range'] * np.sin(np.deg2rad(target['angle']))
        y = target['range'] * np.cos(np.deg2rad(target['angle']))
        z = 0  # 假设目标在同一高度平面
        
        ax.scatter(x, y, z, c='r', s=100, marker='o')
        ax.text(x, y, z, f"v={target['velocity']:.1f}m/s", fontsize=10)
    
    ax.set_xlabel('X (m)')
    ax.set_ylabel('Y (m)')
    ax.set_zlabel('Z (m)')
    ax.set_title('雷达点云')
    ax.set_xlim([-100, 100])
    ax.set_ylim([0, 200])
    ax.set_zlim([-5, 5])
    
    plt.tight_layout()
    plt.show()

if __name__ == "__main__":
    main()    