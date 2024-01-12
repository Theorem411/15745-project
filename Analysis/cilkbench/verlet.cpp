#include <iostream>
#include <cstdlib>
#include <ctime>
#include <cilk/cilk.h>
#include <chrono>
#include <cmath>

struct Particle {
    double x, y, z;  // Position
    double vx, vy, vz;  // Velocity
    double ax, ay, az;  // Acceleration
};

const double dt = 0.001;  // Time step

class VerletSimulation {
public:
    VerletSimulation(int numParticles) {
        particles = new Particle[numParticles];
        numParticles_ = numParticles;
        initializeParticles();
    }

    ~VerletSimulation() {
        delete[] particles;
    }

    void simulate(int numIterations) {
        auto start = std::chrono::high_resolution_clock::now();

        for (int iter = 0; iter < numIterations; ++iter) {
            // Compute forces and update positions in parallel
            cilk_for (int i = 0; i < numParticles_; ++i) {
                particles[i].ax = 0.0;
                particles[i].ay = 0.0;
                particles[i].az = 0.0;

                for (int j = 0; j < numParticles_; ++j) {
                    if (i != j) {
                        // Calculate forces (simplified for illustration)
                        double dx = particles[j].x - particles[i].x;
                        double dy = particles[j].y - particles[i].y;
                        double dz = particles[j].z - particles[i].z;
                        double distanceSquared = dx * dx + dy * dy + dz * dz + 1.0;  // Avoid division by zero
                        double distance = std::sqrt(distanceSquared);

                        double force = 1.0 / distanceSquared;  // Simplified force calculation
                        double fx = force * dx / distance;
                        double fy = force * dy / distance;
                        double fz = force * dz / distance;

                        particles[i].ax += fx;
                        particles[i].ay += fy;
                        particles[i].az += fz;
                    }
                }

                // Update positions and velocities
                particles[i].vx += particles[i].ax * dt;
                particles[i].vy += particles[i].ay * dt;
                particles[i].vz += particles[i].az * dt;

                particles[i].x += particles[i].vx * dt;
                particles[i].y += particles[i].vy * dt;
                particles[i].z += particles[i].vz * dt;
            }
        }

        auto end = std::chrono::high_resolution_clock::now();
        std::chrono::duration<double> duration = end - start;
        std::cout << "Verlet Integration Simulation Time: " << duration.count() << " seconds\n";
    }

private:
    Particle* particles;
    int numParticles_;

    void initializeParticles() {
        // Seed for random number generation
        std::srand(std::time(0));

        // Initialize particles with random positions and velocities
        for (int i = 0; i < numParticles_; ++i) {
            particles[i].x = static_cast<double>(rand()) / RAND_MAX;
            particles[i].y = static_cast<double>(rand()) / RAND_MAX;
            particles[i].z = static_cast<double>(rand()) / RAND_MAX;

            particles[i].vx = static_cast<double>(rand()) / RAND_MAX;
            particles[i].vy = static_cast<double>(rand()) / RAND_MAX;
            particles[i].vz = static_cast<double>(rand()) / RAND_MAX;
        }
    }
};

int main() {
    const int numParticles = 500;
    VerletSimulation verletSimulation(numParticles);

    // Perform simulation with Verlet integration
    verletSimulation.simulate(30);  

    return 0;
}
