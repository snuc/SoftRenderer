#include "stdafx.h"
#include "RenderList.h"


RenderList::RenderList()
{
}


RenderList::~RenderList()
{
}

void RenderList::ResetList()
{
	polyCount = 0;
}

void RenderList::InsertGameObject(GameObject obj)
{
	for (int i = 0; i < obj.polyCount; i++)
	{
		Poly poly = obj.polyList[i];

		RenderPoly rpoly;
		rpoly.State = POLY_STATE_ACTIVE;
		rpoly.texture = poly.texture;
		rpoly.vertices[0] = poly.vertices[poly.vertexIndexs[0]];
		rpoly.vertices[1] = poly.vertices[poly.vertexIndexs[1]];
		rpoly.vertices[2] = poly.vertices[poly.vertexIndexs[2]];

		rpoly.vertices[0].color = obj.vertexListLocal[poly.vertexIndexs[0]].color;
		rpoly.vertices[1].color = obj.vertexListLocal[poly.vertexIndexs[1]].color;
		rpoly.vertices[2].color = obj.vertexListLocal[poly.vertexIndexs[2]].color;

		rpoly.vertices[0].uv = obj.vertexListLocal[poly.vertexIndexs[0]].uv;
		rpoly.vertices[1].uv = obj.vertexListLocal[poly.vertexIndexs[1]].uv;
		rpoly.vertices[2].uv = obj.vertexListLocal[poly.vertexIndexs[2]].uv;
		polyList[polyCount++] = rpoly;
	}
}

void RenderList::DeleteBackface(Camera camera) // 把物体背面的多边形标记为POLY_STATE_BACKFACE
{
	for (int i = 0; i < polyCount; ++i)
	{
		RenderPoly poly = polyList[i];

		Vector4 u = poly.vertices[1].position - poly.vertices[0].position;
		u.w = 1.0f;

		Vector4 v = poly.vertices[2].position - poly.vertices[0].position;
		v.w = 1.0f;

		Vector4 n = Vector4::crossProduct(u, v);


		Vector4 view = camera.position - poly.vertices[0].position;
		view.w = 1.0f;

		float dp = n.dotProduct(view);

		if (dp <= 0.0f)
		{
			polyList[i].State = POLY_STATE_BACKFACE;
		}
		else
		{
			polyList[i].State = POLY_STATE_ACTIVE;
		}
	}
}

void RenderList::CameraTransform(Camera camera)
{
	Matrix4 mt = camera.MatrixCamera;

	for (int i = 0; i < polyCount; ++i)
	{
		if (polyList[i].State != POLY_STATE_ACTIVE)
			continue;

		for (int j = 0; j < 3; j++)
		{
			polyList[i].vertices[j].position = polyList[i].vertices[j].position * mt;
		}

	}
}

void RenderList::ClipPoly(Camera camera)
{
#define CLIP_CODE_GZ 0x0001	// z > z_max
#define CLIP_CODE_LZ 0x0002 // z < z_min
#define CLIP_CODE_IZ 0x0004 // z_min < z < z_max

#define CLIP_CODE_GX 0x0001 // x > x_max
#define CLIP_CODE_LX 0x0002 // x < x_min
#define CLIP_CODE_IX 0x0004 // x_min < x < x_max

#define CLIP_CODE_GY 0x0001 // y > y_max
#define CLIP_CODE_LY 0x0002 // y < y_min
#define CLIP_CODE_IY 0x0004 // y_min < y < y_max

	int vertex_ccodes[3];
	int num_verts_in;
	int v0, v1, v2;

	float z_factor, z_test;

	float xi, yi, x01i, y01i, x02i, y02i, t1, t2;
	float ui, vi, u01i, v01i, u02i, v02i;

	int last_poly_index = polyCount;

	Vector4 u, v, n;

	Vector4 temp_position;

	RenderPoly temp_poly;

	for (int i = 0; i < last_poly_index; i++)
	{
		RenderPoly current_poly = polyList[i];
		if (current_poly.State != POLY_STATE_ACTIVE)
			continue;

		//x planes
		{
			z_factor = (0.5f) * camera.ViewPlaneWidth / camera.ViewDistance;

			for (int j = 0; j < 3; j++)
			{
				temp_position = current_poly.vertices[j].position;
				z_test = z_factor * temp_position.z;

				if (temp_position.x > z_test) {
					vertex_ccodes[j] = CLIP_CODE_GX;
				}
				else if (temp_position.x < -z_test) {
					vertex_ccodes[j] = CLIP_CODE_LX;
				}
				else {
					vertex_ccodes[j] = CLIP_CODE_IX;
				}
			}

			if ((vertex_ccodes[0] == CLIP_CODE_GX && vertex_ccodes[1] == CLIP_CODE_GX && vertex_ccodes[2] == CLIP_CODE_GX) ||
				(vertex_ccodes[0] == CLIP_CODE_LX && vertex_ccodes[1] == CLIP_CODE_LX && vertex_ccodes[2] == CLIP_CODE_LX))
			{
				polyList[i].State = POLY_STATE_CLIPPED;
				continue;
			}
		}

		//y planes
		{
			z_factor = (0.5f) * camera.ViewPlaneWidth / camera.ViewDistance;

			for (int j = 0; j < 3; j++)
			{
				temp_position = current_poly.vertices[j].position;
				z_test = z_factor * temp_position.z;

				if (temp_position.y > z_test) {
					vertex_ccodes[j] = CLIP_CODE_GY;
				}
				else if (temp_position.y < -z_test) {
					vertex_ccodes[j] = CLIP_CODE_LY;
				}
				else {
					vertex_ccodes[j] = CLIP_CODE_IY;
				}
			}

			if ((vertex_ccodes[0] == CLIP_CODE_GY && vertex_ccodes[1] == CLIP_CODE_GY && vertex_ccodes[2] == CLIP_CODE_GY) ||
				(vertex_ccodes[0] == CLIP_CODE_LY && vertex_ccodes[1] == CLIP_CODE_LY && vertex_ccodes[2] == CLIP_CODE_LY))
			{
				polyList[i].State = POLY_STATE_CLIPPED;
				continue;
			}
		}

		//z planes
		{
			num_verts_in = 0;

			for (int j = 0; j < 3; j++)
			{
				temp_position = current_poly.vertices[j].position;

				if (temp_position.z > camera.FarZ)
				{
					vertex_ccodes[j] = CLIP_CODE_GZ;
				}
				else if (temp_position.z < camera.NearZ)
				{
					vertex_ccodes[j] = CLIP_CODE_LZ;
				}
				else {
					vertex_ccodes[j] = CLIP_CODE_IZ;
					num_verts_in++;
				}
			}

			if ((vertex_ccodes[0] == CLIP_CODE_GZ && vertex_ccodes[1] == CLIP_CODE_GZ && vertex_ccodes[2] == CLIP_CODE_GZ) ||
				(vertex_ccodes[0] == CLIP_CODE_LZ && vertex_ccodes[1] == CLIP_CODE_LZ && vertex_ccodes[2] == CLIP_CODE_LZ))
			{
				polyList[i].State = POLY_STATE_CLIPPED;
				continue;
			}

			if ((vertex_ccodes[0] | vertex_ccodes[1] | vertex_ccodes[2]) & CLIP_CODE_LZ)
			{
				if (num_verts_in == 1)
				{
					if (vertex_ccodes[0] == CLIP_CODE_IZ)
					{
						v0 = 0; v1 = 1; v2 = 2;
					}
					else if (vertex_ccodes[1] == CLIP_CODE_IZ)
					{
						v0 = 1; v1 = 2; v2 = 0;
					}
					else
					{
						v0 = 2; v1 = 0; v2 = 1;
					}

					// clip edge v0->v1
					v = Vector4::buildSegment(current_poly.vertices[v0].position, current_poly.vertices[v1].position);

					// the intersection occurs when z = near z, so t = 
					t1 = (camera.NearZ - current_poly.vertices[v0].position.z) / v.z;

					// now plug t back in and find x,y intersection with the plane
					xi = current_poly.vertices[v0].position.x + v.x * t1;
					yi = current_poly.vertices[v0].position.y + v.y * t1;

					// now overwrite vertex with new vertex
					current_poly.vertices[v1].position.x = xi;
					current_poly.vertices[v1].position.y = yi;
					current_poly.vertices[v1].position.z = camera.NearZ;

					
					// clip edge v0->v2
					v = Vector4::buildSegment(current_poly.vertices[v0].position, current_poly.vertices[v2].position);

					// the intersection occurs when z = near z, so t = 
					t2 = (camera.NearZ - current_poly.vertices[v0].position.z) / v.z;

					// now plug t back in and find x,y intersection with the plane
					xi = current_poly.vertices[v0].position.x + v.x * t2;
					yi = current_poly.vertices[v0].position.y + v.y * t2;

					// now overwrite vertex with new vertex
					current_poly.vertices[v2].position.x = xi;
					current_poly.vertices[v2].position.y = yi;
					current_poly.vertices[v2].position.z = camera.NearZ;

					// now that we have both t1, t2, check if the poly is textured, if so clip
					// texture coordinates
					if (current_poly.texture.HasRead())
					{
						ui = current_poly.vertices[v0].uv.x + (current_poly.vertices[v1].uv.x - current_poly.vertices[v0].uv.x) * t1;
						vi = current_poly.vertices[v0].uv.y + (current_poly.vertices[v1].uv.y - current_poly.vertices[v0].uv.y) * t1;

						current_poly.vertices[v1].uv.x = ui;
						current_poly.vertices[v1].uv.y = vi;

						ui = current_poly.vertices[v0].uv.x + (current_poly.vertices[v2].uv.x - current_poly.vertices[v0].uv.x) * t2;
						vi = current_poly.vertices[v0].uv.y + (current_poly.vertices[v2].uv.y - current_poly.vertices[v0].uv.y) * t2;

						current_poly.vertices[v2].uv.x = ui;
						current_poly.vertices[v2].uv.y = vi;
					} // end if textured

					polyList[i] = current_poly;
				}
				else if (num_verts_in == 2)
				{
					temp_poly = current_poly;
					
					// 找出位于外侧的顶点
					if (vertex_ccodes[0] == CLIP_CODE_LZ)
					{
						v0 = 0; v1 = 1; v2 = 2;
					}
					else if (vertex_ccodes[1] == CLIP_CODE_LZ)
					{
						v0 = 1; v1 = 2; v2 = 0;
					}
					else
					{
						v0 = 2; v1 = 0; v2 = 1;
					}

					// clip edge v0->v1
					v = Vector4::buildSegment(current_poly.vertices[v0].position, current_poly.vertices[v1].position);

					// the intersection occurs when z = near z, so t = 
					t1 = (camera.NearZ - current_poly.vertices[v0].position.z) / v.z;

					// now plug t back in and find x,y intersection with the plane
					x01i = current_poly.vertices[v0].position.x + v.x * t1;
					y01i = current_poly.vertices[v0].position.y + v.y * t1;

					// clip edge v0->v2
					v = Vector4::buildSegment(current_poly.vertices[v0].position, current_poly.vertices[v2].position);

					// the intersection occurs when z = near z, so t = 
					t2 = (camera.NearZ - current_poly.vertices[v0].position.z) / v.z;

					// now plug t back in and find x,y intersection with the plane
					x02i = current_poly.vertices[v0].position.x + v.x * t2;
					y02i = current_poly.vertices[v0].position.y + v.y * t2;

					// now we have both intersection points, we must overwrite the inplace
					// polygon's vertex 0 with the intersection point, this poly 1 of 2 from
					// the split

					// now overwrite vertex with new vertex
					current_poly.vertices[v0].position.x = x01i;
					current_poly.vertices[v0].position.y = y01i;
					current_poly.vertices[v0].position.z = camera.NearZ;

					// now comes the hard part, we have to carefully create a new polygon
					// from the 2 intersection points and v2, this polygon will be inserted
					// at the end of the rendering list, but for now, we are building it up
					// in  temp_poly

					// so leave v2 alone, but overwrite v1 with v01, and overwrite v0 with v02
					temp_poly.vertices[v1].position.x = x01i;
					temp_poly.vertices[v1].position.y = y01i;
					temp_poly.vertices[v1].position.z = camera.NearZ;

					temp_poly.vertices[v0].position.x = x02i;
					temp_poly.vertices[v0].position.y = y02i;
					temp_poly.vertices[v0].position.z = camera.NearZ;

					// now that we have both t1, t2, check if the poly is textured, if so clip
					// texture coordinates
					if (current_poly.texture.HasRead())
					{
						// compute poly 1 new texture coordinates from split
						u01i = current_poly.vertices[v0].uv.x + (current_poly.vertices[v1].uv.x - current_poly.vertices[v0].uv.x) * t1;
						v01i = current_poly.vertices[v0].uv.y + (current_poly.vertices[v1].uv.y - current_poly.vertices[v0].uv.y) * t1;

						// compute poly 2 new texture coordinates from split
						u02i = current_poly.vertices[v0].uv.x + (current_poly.vertices[v2].uv.x - current_poly.vertices[v0].uv.x) * t2;
						v02i = current_poly.vertices[v0].uv.y + (current_poly.vertices[v2].uv.y - current_poly.vertices[v0].uv.y) * t2;

						// write them all at the same time         
						// poly 1
						current_poly.vertices[v0].uv.x = u01i;
						current_poly.vertices[v0].uv.y = v01i;

						// poly 2
						temp_poly.vertices[v0].uv.x = u02i;
						temp_poly.vertices[v0].uv.y = v02i;
						temp_poly.vertices[v1].uv.x = u01i;
						temp_poly.vertices[v1].uv.y = v01i;
					} // end if textured

					polyList[i] = current_poly;
					polyList[polyCount++] = temp_poly;
				}
			}
		}
	}
}

void RenderList::ProcessLight(Camera camera, Light ambientLight, Light directionalLight)//gouraud mode
{
	ColourValue c0, c1, c2;

	uint8 r_base, g_base, b_base,  // base color being lit
		ri, gi, bi;            // final color

	uint32 r_sum, g_sum, b_sum;   // sum of lighting process over all lights

	for (int i = 0; i < polyCount; i++)
	{
		RenderPoly poly = polyList[i];

		if (poly.State != POLY_STATE_ACTIVE)
		{
			continue;
		}

		for (int j = 0; j < 3; j++)
		{
			r_sum = 0;
			g_sum = 0;
			b_sum = 0;

			ColourValue color = poly.texture.HasRead() ? ColourValue::White : poly.vertices[j].color;

			/*
			Vector4 u = vertexListTrans[poly.vertexIndexs[1]].position - vertexListTrans[poly.vertexIndexs[0]].position;
			u.w = 1.0f;

			Vector4 v = vertexListTrans[poly.vertexIndexs[2]].position - vertexListTrans[poly.vertexIndexs[0]].position;
			v.w = 1.0f;

			Vector4 n = Vector4::crossProduct(u, v);
			*/

			if (ambientLight.state == LIGHTV1_STATE_ON)
			{
				//环境光

				ri = static_cast<uint8>((ambientLight.c_ambient.r * color.r) * 255);
				gi = static_cast<uint8>((ambientLight.c_ambient.g * color.g) * 255);
				bi = static_cast<uint8>((ambientLight.c_ambient.b * color.b) * 255);

				// ambient light has the same affect on each vertex
				r_sum += ri;
				g_sum += gi;
				b_sum += bi;
			}

			if (directionalLight.state == LIGHTV1_STATE_ON)
			{
				//直射光

				//float dp = directionalLight.dir.dotProduct(n);
				float dp = directionalLight.dir.dotProduct(poly.vertices[j].normal);
				if (dp > 0.0f)
				{
					ri = static_cast<uint8>((directionalLight.c_diffuse.r * color.r * dp) * 255);
					gi = static_cast<uint8>((directionalLight.c_diffuse.g * color.g * dp) * 255);
					bi = static_cast<uint8>((directionalLight.c_diffuse.b * color.b * dp) * 255);

					r_sum += ri;
					g_sum += gi;
					b_sum += bi;
				}
			}

			if (r_sum  > 255) r_sum = 255;
			if (g_sum  > 255) g_sum = 255;
			if (b_sum  > 255) b_sum = 255;


			polyList[i].vertices[j].color = ColourValue(r_sum / 255.0f, g_sum / 255.0f, b_sum / 255.0f);
		}
	}
}

void RenderList::ProjectTransform(Camera camera) // 透视变换，将3D坐标透视为2D坐标，结果为x取值为(-1,1)，Y取值为(-1/ar,1/ar)
{
	for (int i = 0; i < polyCount; i++)
	{
		if (polyList[i].State != POLY_STATE_ACTIVE)
		{
			continue;
		}

		for (int j = 0; j < 3; j++)
		{
			polyList[i].vertices[j].position.x = polyList[i].vertices[j].position.x * camera.ViewDistance / polyList[i].vertices[j].position.z;
			polyList[i].vertices[j].position.y = polyList[i].vertices[j].position.y * camera.ViewDistance * camera.AspectRatio / polyList[i].vertices[j].position.z;
		}
	}
}

void RenderList::ScreenTransform(Camera camera) // 视口变换，结果取值X：(0,SCREEN_WIDTH) Y：(0,SCREEN_HEIGHT)
{
	float alpha = 0.5 * camera.ScreenWidth - 0.5;
	float beta = 0.5 * camera.ScreenHeight - 0.5;
	for (int i = 0; i < polyCount; i++)
	{
		if (polyList[i].State != POLY_STATE_ACTIVE)
		{
			continue;
		}

		for (int j = 0; j < 3; j++)
		{
			polyList[i].vertices[j].position.x = alpha + alpha * polyList[i].vertices[j].position.x;
			polyList[i].vertices[j].position.y = beta - beta * polyList[i].vertices[j].position.y;
		}
	}
}