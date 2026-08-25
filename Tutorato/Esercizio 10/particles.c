#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#define N 32768
#define DT 0.01f
#define EPS 1e-9f

struct _particle {
  float x, y, z;
  float vx, vy, vz;
  float mass;
};

typedef struct _particle particle;

void init_particles(particle * p, int n)
{
  for (int i = 0; i < n; i++) {
    p[i].x = (float)rand() / RAND_MAX;
    p[i].y = (float)rand() / RAND_MAX;
    p[i].z = (float)rand() / RAND_MAX;
    p[i].vx = 0.0f;
    p[i].vy = 0.0f;
    p[i].vz = 0.0f;
    p[i].mass = 1.0f;
  }
}

void update_particles(particle * particles, int n)
{
  for (int i = 0; i < n; i++) {
    float fx = 0.0f;
    float fy = 0.0f;
    float fz = 0.0f;
        
    for (int j = 0; j < n; j++) {
      float dx = particles[j].x - particles[i].x;
      float dy = particles[j].y - particles[i].y;
      float dz = particles[j].z - particles[i].z;
            
      float distSqr = dx*dx + dy*dy + dz*dz + EPS;
      float invDist = 1.0f / sqrtf(distSqr);
      float invDist3 = invDist * invDist * invDist;
            
      float f = particles[j].mass * invDist3;
      fx += dx * f;
      fy += dy * f;
      fz += dz * f;
    }
        
    particles[i].vx += DT * fx;
    particles[i].vy += DT * fy;
    particles[i].vz += DT * fz;
  }

  for (int i = 0; i < n; i++) {
    particles[i].x += DT * particles[i].vx;
    particles[i].y += DT * particles[i].vy;
    particles[i].z += DT * particles[i].vz;
  }
}

int main()
{
  particle *particles = malloc(N * sizeof(particle));
  init_particles(particles, N);

  update_particles(particles, N);

  printf("Sample output (P0 pos): %f, %f, %f\n",
	 particles[0].x, particles[0].y, particles[0].z);

  free(particles);
  return 0;
}
