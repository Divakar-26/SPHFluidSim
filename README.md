# Particle Simulation using SPH (Smooth Particle Hydrodynamics)

This project is a **2D particle-based fluid simulation** implemented in C++ using **SDL3** and **OpenGL**. The simulation demonstrates the dynamics of particles interacting under pressure, gravity, and user-applied forces. It also includes a GUI via **ImGui** for live tweaking of parameters.

## Formula Used

### 1. SPH Interpolation

SPH allows us to estimate any field quantity (e.g., temperature, velocity, or some property) at a given point using neighboring particles:

![Interpolation Equation](images/interpolation.png)

- \( A(\mathbf{r}) \) → interpolated property at point \( \mathbf{r} \)  
- \( A_j \) → property of particle \( j \)  
- \( m_j \) → mass of particle \( j \)  
- \( \rho_j \) → density of particle \( j \)  
- \( W(|\mathbf{r} - \mathbf{r}_j|, h) \) → smoothing kernel (Poly6 in 2D)  
- \( h \) → smoothing radius 

**Example from the simulation code:**

```cpp
for (int i = 0; i < numParticles; i++) {
    glm::vec2 vec = point - position[i];
    float dst = glm::length(vec);

    if (densities[i] > 0.0f && dst < smoothingRadius) {
        float weight = smoothingKernel(smoothingRadius, dst);
        property += -(properties[i] * (mass / densities[i])) * weight;
    }
} 
```

### 2. Density Calculation

For each particle, density is calculated based on neighboring particles within the smoothing radius `sr`:

\[
\rho_i = \sum_j m_j W(|\mathbf{r}_i - \mathbf{r}_j|, h)
\]

- \( \rho_i \) : density of particle \(i\)  
- \( m_j \) : mass of neighboring particle \(j\) (here `1.0`)  
- \( W \) : smoothing kernel function  
- \( h \) : smoothing radius (`smoothingRadius`)  


### 3. Pressure Calculation

Pressure is calculated from density deviation from a target density:

\[
P_i = k (\rho_i - \rho_0)
\]

- \( P_i \) : pressure of particle \(i\)  
- \( k \) : pressure multiplier (`pressureMultiplier`)  
- \( \rho_0 \) : target density (`targetDensity`) 

### 4. Pressure Force

The pressure force applied to each particle is:

\[
\mathbf{F}_i^\text{pressure} = -\sum_j m_i m_j \frac{P_i + P_j}{2} \left(\frac{1}{\rho_i} + \frac{1}{\rho_j}\right) \nabla W(\mathbf{r}_i - \mathbf{r}_j, h)
\]

- \( \nabla W \) : gradient of the smoothing kernel.

### 5. Smoothing Kernel (Poly6 Kernel in 2D)

The smoothing kernel describes how much a particle influences its neighbors based on distance. In SPH, every property (like density, pressure, or force) is weighted by this kernel.

We use the **Poly6 kernel** for 2D:

\[
W(r, h) = \frac{4}{\pi h^8} (h^2 - r^2)^3, \quad 0 \le r \le h
\]

- \( r = |\mathbf{r}_i - \mathbf{r}_j| \)  
- \( h \) : smoothing radius  

Gradient of the kernel:

\[
\nabla W(r, h) = -\frac{30}{\pi h^5} (h - r)^2 \frac{\mathbf{r}}{r}, \quad 0 < r \le h
\]

This ensures smooth forces and prevents particle clustering.