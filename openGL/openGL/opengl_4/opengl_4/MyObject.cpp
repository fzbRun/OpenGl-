#include"MyObject.h"

MyObject::MyObject(vec3 position, float* vertices, int num, vec3 size, vec3 albedo, vec4 rotate) {

	this->position = position;
	this->vertices = vertices;
	this->num = num;
	this->size = size;
	this->albedo = albedo;
	this->rotate = rotate;

	//对物体每个顶点与法线进行旋转，缩放，平移
	for (int i = 0; i < num * 8; i += 8) {

		//如果不需要旋转那么我们就直接算减少性能消耗
		if (rotate == vec4(0.0f)) {
			//顶点
			vertices[i] *= size.x;
			vertices[i+1] *= size.y;
			vertices[i+2] *= size.z;
			vertices[i] += position.x;
			vertices[i+1] += position.y;
			vertices[i+2] += position.z;
		}
		else {

			vec3 point = vec3(vertices[i], vertices[i + 1], vertices[i + 2]);
			mat4 model;
			model = translate(model, position);
			model = scale(model, size);
			model = glm::rotate(model, radians(rotate.w), vec3(rotate));
			point = vec3(model * vec4(point, 1.0f));
			//顶点
			vertices[i] = point.x;
			vertices[i+1] = point.y;
			vertices[i+2] = point.z;
			//法线
			model = transpose(inverse(model));
			vec3 normal = vec3(vertices[i + 3], vertices[i + 4], vertices[i + 5]);
			normal = vec3(model * vec4(normal,1.0f));
			vertices[i + 3] = normal.x;
			vertices[i + 4] = normal.y;
			vertices[i + 5] = normal.z;
		}

	}

	makeBox();

}

void MyObject::makeBox() {

	float maxX = (float)-DBL_MAX;
	float minX = (float)DBL_MAX;
	float maxY = (float)-DBL_MAX;
	float minY = (float)DBL_MAX;
	float maxZ = (float)-DBL_MAX;
	float minZ = (float)DBL_MAX;

	for (int i = 0; i < num * 8; i+=8) {

		if (vertices[i] > maxX) {
			maxX = vertices[i];
		}
		else if (vertices[i] < minX) {
			minX = vertices[i];
		}

		if (vertices[i+1] > maxY) {
			maxY = vertices[i+1];
		}
		else if (vertices[i+1] < minY) {
			minY = vertices[i+1];
		}

		if (vertices[i+2] > maxZ) {
			maxZ = vertices[i+2];
		}
		else if (vertices[i+2] < minZ) {
			minZ = vertices[i+2];
		}

	}

	box = new float[6]{ minX, minY, minZ, maxX, maxY, maxZ };

}