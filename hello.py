import numpy as np
import matplotlib.pyplot as plt
import math

def smoothing_kernel(sr, r):
    """
    Poly6 kernel for 2D
    coef = 4.0f / (M_PI * pow(sr, 8))
    return coef * (sr^2 - r^2)^3
    """
    if r >= sr:
        return 0.0
    coef = 4.0 / (math.pi * (sr ** 8))
    diff = (sr * sr - r * r)
    return coef * diff * diff * diff

# Parameters
sr = 1.0  # smoothing radius
r_values = np.linspace(0, 1.5, 500)  # r from 0 to 1.5

# Calculate kernel values
kernel_values = [smoothing_kernel(sr, r) for r in r_values]

# Create the plot
plt.figure(figsize=(10, 6))
plt.plot(r_values, kernel_values, 'b-', linewidth=2, label='Kernel Value')
plt.axvline(x=sr, color='r', linestyle='--', label=f'Smoothing Radius (sr = {sr})')
plt.axhline(y=0, color='k', linestyle='-', alpha=0.3)

# Add labels and title
plt.xlabel('Distance (r)')
plt.ylabel('Kernel Value')
plt.title('Poly6 Smoothing Kernel (2D)\n$W(r) = \\frac{4}{\\pi h^8}(h^2 - r^2)^3$')
plt.grid(True, alpha=0.3)
plt.legend()

# Add some annotations
max_value = max(kernel_values)
max_index = np.argmax(kernel_values)
plt.annotate(f'Max value: {max_value:.3f} at r=0', 
             xy=(0, max_value), 
             xytext=(0.2, max_value*0.8),
             arrowprops=dict(arrowstyle='->', color='green'),
             fontsize=10)

plt.annotate('Kernel = 0 for r ≥ h', 
             xy=(sr, 0), 
             xytext=(sr+0.1, max_value*0.3),
             arrowprops=dict(arrowstyle='->', color='red'),
             fontsize=10)

plt.tight_layout()
plt.show()

# Print some key values
print("Kernel values at key distances:")
print(f"r = 0.0: {smoothing_kernel(sr, 0.0):.6f}")
print(f"r = 0.25: {smoothing_kernel(sr, 0.25):.6f}")
print(f"r = 0.5: {smoothing_kernel(sr, 0.5):.6f}")
print(f"r = 0.75: {smoothing_kernel(sr, 0.75):.6f}")
print(f"r = 1.0: {smoothing_kernel(sr, 1.0):.6f}")
print(f"r = 1.1: {smoothing_kernel(sr, 1.1):.6f}")

# Verify normalization (should integrate to 1 in 2D)
def integrate_kernel_2d(sr, num_samples=1000):
    """Numerically integrate the kernel in 2D"""
    r_max = sr
    dr = r_max / num_samples
    total = 0
    for i in range(num_samples):
        r = i * dr
        area = 2 * math.pi * r * dr  # area of ring at radius r
        kernel_val = smoothing_kernel(sr, r)
        total += kernel_val * area
    return total

integral = integrate_kernel_2d(sr)
print(f"\n2D integral (should be close to 1): {integral:.6f}")