#version 430 core

#define NUM_BLADES 4

layout (triangles, invocations = 12) in;

layout (triangle_strip, max_vertices = 24) out;

uniform mat4 vp;
uniform mat4 model;
uniform float time;

uniform vec3 cameraPos;

#define MAX_LIGHTS 8

uniform vec3 lightDir[MAX_LIGHTS];
uniform int numDirectionalLights = 0;

in vec3 surface_normal[];

out vec3 normal_GS;
out vec3 occlusionFactor;

vec4 permute(vec4 x){return mod(((x*34.0)+1.0)*x, 289.0);}
vec2 fade(vec2 t) {return t*t*t*(t*(t*6.0-15.0)+10.0);}

float cnoise(vec2 P){
  vec4 Pi = floor(P.xyxy) + vec4(0.0, 0.0, 1.0, 1.0);
  vec4 Pf = fract(P.xyxy) - vec4(0.0, 0.0, 1.0, 1.0);
  Pi = mod(Pi, 289.0); // To avoid truncation effects in permutation
  vec4 ix = Pi.xzxz;
  vec4 iy = Pi.yyww;
  vec4 fx = Pf.xzxz;
  vec4 fy = Pf.yyww;
  vec4 i = permute(permute(ix) + iy);
  vec4 gx = 2.0 * fract(i * 0.0243902439) - 1.0; // 1/41 = 0.024...
  vec4 gy = abs(gx) - 0.5;
  vec4 tx = floor(gx + 0.5);
  gx = gx - tx;
  vec2 g00 = vec2(gx.x,gy.x);
  vec2 g10 = vec2(gx.y,gy.y);
  vec2 g01 = vec2(gx.z,gy.z);
  vec2 g11 = vec2(gx.w,gy.w);
  vec4 norm = 1.79284291400159 - 0.85373472095314 * 
    vec4(dot(g00, g00), dot(g01, g01), dot(g10, g10), dot(g11, g11));
  g00 *= norm.x;
  g01 *= norm.y;
  g10 *= norm.z;
  g11 *= norm.w;
  float n00 = dot(g00, vec2(fx.x, fy.x));
  float n10 = dot(g10, vec2(fx.y, fy.y));
  float n01 = dot(g01, vec2(fx.z, fy.z));
  float n11 = dot(g11, vec2(fx.w, fy.w));
  vec2 fade_xy = fade(Pf.xy);
  vec2 n_x = mix(vec2(n00, n01), vec2(n10, n11), fade_xy.x);
  float n_xy = mix(n_x.x, n_x.y, fade_xy.y);
  return 2.3 * n_xy;
}

float rand(vec2 co){
    return mod(fract(sin(dot(co.xy ,vec2(12.9898,78.233))) * 43758.5453), 1.0);
}

void main()
{
	vec3 middle = vec3(0.0, 0.0, 0.0);

	for(int i = 0; i < 3; ++i)
	{
		middle += gl_in[i].gl_Position.xyz;
	}	

	middle /= 3.f;

	float dx = 0.3*cnoise(vec2(middle.x, 0.5*time));	
	float dz = 0.3*cnoise(vec2(middle.z, 0.5*time));

	float cameraDistance = length(cameraPos - vec3(model*vec4(middle, 1.0)));

	if(middle.y >= 12 && middle.y < 22 && (cameraDistance < 50))
	{
		int num_blades = 1;
		
		if(cameraDistance < 20)
		{
			num_blades = 2;
		}

		// TODO: We can fetch the number of blades to generate from a texture
		for(int i = 0; i < NUM_BLADES; ++i)
		{
			// http://mathworld.wolfram.com/TrianglePointPicking.html

			vec3 p0 = gl_in[0].gl_Position.xyz;
			vec3 p1 = gl_in[1].gl_Position.xyz;
			vec3 p2 = gl_in[2].gl_Position.xyz;

			vec3 v0 = p1 - p0;
			vec3 v1 = p2 - p0;

			float rand_seed = rand(p0.xz + vec2(gl_InvocationID, 0.0));

			float a = rand(vec2(rand_seed, 3 * i + 0));
			float b = rand(vec2(rand_seed, 3 * i + 1));
			float rand_rot = 360 * rand(vec2(rand_seed, 3 * i + 2));

			mat3 rot_matrix = mat3(
				cos(rand_rot), 0, sin(rand_rot),
				0, 1, 0,
				-sin(rand_rot), 0, cos(rand_rot));

			if(a + b > 1)
			{
				a = 1 - a;
				b = 1 - b;
			}
		
			vec3 p = p0 + a * v0 + b * v1;

			vec3 pp0 = rot_matrix * vec3(0.02, 0.0, 0.0);
			vec3 pp1 = rot_matrix * vec3(-0.02, 0.0, 0.0);
			vec3 pp2 = rot_matrix * 0.4 * vec3(dx, 1.0, dz);

			vec3 norm = normalize(surface_normal[0]);
			vec3 factor = vec3(0.0, 0.0, 0.0);

			for(int i = 0; i < numDirectionalLights; ++i)
			{
				vec3 lightDirNorm = normalize(lightDir[i]);

				float diffFactor = max(dot(norm, -lightDirNorm), 0.0);

				factor += vec3(diffFactor);
			}

			gl_Position = vp * model * vec4(p + pp0, 1.0);
			normal_GS = normalize(mat3(inverse(transpose(model))) * normalize(cross(pp1-pp0, pp2-pp0)));
			occlusionFactor = factor;
			EmitVertex();
			gl_Position = vp * model * vec4(p + pp1, 1.0);
			normal_GS = normalize(mat3(inverse(transpose(model))) * normalize(cross(pp1-pp0, pp2-pp0)));
			occlusionFactor = factor;
			EmitVertex();
			gl_Position = vp * model * vec4(p + pp2, 1.0);
			normal_GS = normalize(mat3(inverse(transpose(model))) * normalize(cross(pp1-pp0, pp2-pp0)));
			occlusionFactor = factor;
			EmitVertex();
			EndPrimitive();

			gl_Position = vp * model * vec4(p + pp1, 1.0);
			normal_GS = normalize(mat3(inverse(transpose(model))) * normalize(cross(pp2-pp0, pp1-pp0)));
			occlusionFactor = factor;
			EmitVertex();
			gl_Position = vp * model * vec4(p + pp0, 1.0);
			normal_GS = normalize(mat3(inverse(transpose(model))) * normalize(cross(pp2-pp0, pp1-pp0)));
			occlusionFactor = factor;
			EmitVertex();
			gl_Position = vp * model * vec4(p + pp2, 1.0);
			normal_GS = normalize(mat3(inverse(transpose(model))) * normalize(cross(pp2-pp0, pp1-pp0)));
			occlusionFactor = factor;
			EmitVertex();
			EndPrimitive();
		}
	}
}