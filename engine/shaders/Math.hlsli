// Shadow map related variables
#define NUM_SAMPLES 20
#define BLOCKER_SEARCH_NUM_SAMPLES NUM_SAMPLES
#define PCF_NUM_SAMPLES NUM_SAMPLES
#define NUM_RINGS 3

#define EPS 1e-3
#define PI 3.141592653589793
#define PI2 6.283185307179586
#define SAMPLE_SIZE 2048

#define NEAR_PLANE (0.5 / 500.0)
#define LIGHT_WORLD_SIZE    0.2
#define LIGHT_FRUSTUM_WIDTH (30.0)
#define LIGHT_SIZE_UV (LIGHT_WORLD_SIZE / LIGHT_FRUSTUM_WIDTH)

float rand_1to1(float x ) { 
    return frac(sin(x)*10000.0);
}

float rand_2to1(float2 uv ) { 
  // 0 - 1
    float a = 12.9898, b = 78.233, c = 43758.5453;
    float dt = dot( uv, float2(a,b) );
    float sn = fmod( dt, PI );
    return frac(sin(sn) * c);
}

void poissonDiskSamples(float2 randomSeed, out float2 poissonDisk[NUM_SAMPLES]) {
  float ANGLE_STEP = PI2 * float( NUM_RINGS ) / float( NUM_SAMPLES );
  float INV_NUM_SAMPLES = 1.0 / float( NUM_SAMPLES );

  float angle = rand_2to1( randomSeed ) * PI2;
  float radius = INV_NUM_SAMPLES;
  float radiusStep = radius;

  for( int i = 0; i < NUM_SAMPLES; i ++ ) {
    poissonDisk[i] = float2( cos( angle ), sin( angle ) ) * pow( radius, 0.75 );
    radius += radiusStep;
    angle += ANGLE_STEP;
  }
}


void uniformDiskSamples(float2 randomSeed, out float2 poissonDisk[NUM_SAMPLES]) {

  float randNum = rand_2to1(randomSeed);
  float sampleX = rand_1to1( randNum ) ;
  float sampleY = rand_1to1( sampleX ) ;

  float angle = sampleX * PI2;
  float radius = sqrt(sampleY);
  
  for( int i = 0; i < NUM_SAMPLES; i ++ ) {
    poissonDisk[i] = float2( radius * cos(angle) , radius * sin(angle)  );

    sampleX = rand_1to1( sampleY ) ;
    sampleY = rand_1to1( sampleX ) ;

    angle = sampleX * PI2;
    radius = sqrt(sampleY);
  }
}

float bias(float3 lightDir, float3 normal)
{
  lightDir = normalize(lightDir);
  normal = normalize(normal);
  return max(0.01 * (1.0 - dot(normal, lightDir)), 0.005); // max(0.001 * (1.0 - dot(normal, lightDir)), 0.005)
}