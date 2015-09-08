#include "stdafx.h"
#include "GameObject.h"
#include "Matrix4.h"

void GameObject::ObjectWorldTransform() // 物体世界变换，使用VertexListLocal作为顶点数据源，顶点变换结果存放在VertexListTrans中
{
	for (int i = 0; i < vertexCount; ++i)
	{
		vertexListTrans[i].position.x = vertexListTrans[i].position.x + worldPos.x;
		vertexListTrans[i].position.y = vertexListTrans[i].position.y + worldPos.y;
		vertexListTrans[i].position.z = vertexListTrans[i].position.z + worldPos.z;
		vertexListTrans[i].position.w = 1;
	}
}

void GameObject::ObjectTransform(Matrix4 mt, int transflag, int basis) // basis是是否对局部坐标轴执行旋转
{
	switch (transflag)
	{
		case RENDER_TRANSFORM_LOCAL:
		{
			for (int i = 0; i < vertexCount; i++)
			{
				//vertexListLocal[i] = vertexListLocal[i] * mt;
				Vector4 result; // 一定要先用缓存保存一个副本，而不是保存在原矩阵，否则影响乘法结果
				result = vertexListLocal[i].position * mt;
				vertexListLocal[i].position = result;
				vertexListLocal[i].normal = vertexListLocal[i].normal * mt;
			}
		} break;
		case RENDER_TRANSFORM_TRANS:
		{
			for (int i = 0; i < vertexCount; i++)
			{
				vertexListTrans[i].position = vertexListTrans[i].position * mt;
				//vertexListTrans[i].normal = vertexListTrans[i].normal * mt;
			}
		} break;
		case RENDER_TRANSFORM_LOCALTOTRANS:
		{
			for (int i = 0; i < vertexCount; i++)
			{
				//vertexListTrans[i] = vertexListLocal[i] * mt;
				Vector4 result; // 一定要先用缓存保存一个副本，而不是保存在原矩阵，否则影响乘法结果
				result = vertexListLocal[i].position * mt;
				vertexListTrans[i].position = result;
				vertexListTrans[i].normal = vertexListLocal[i].normal * mt;
			}
		} break;
		default: break;
	}

}

void GameObject::ComputeVertexNormals()
{
	int polys_touch_vertex[OBJECT_MAX_VERTICES] = { 0 };

	
	for (int poly = 0; poly < polyCount; poly++)
	{

		// extract vertex indices into master list, rember the polygons are 
		// NOT self contained, but based on the vertex list stored in the object
		// itself
		int vindex_0 = polyList[poly].vertexIndexs[0];
		int vindex_1 = polyList[poly].vertexIndexs[1];
		int vindex_2 = polyList[poly].vertexIndexs[2];

		// we need to compute the normal of this polygon face, and recall
		// that the vertices are in cw order, u=p0->p1, v=p0->p2, n=uxv
		//Vector4 u, v, n;

		// build u, v
		Vector4 u = vertexListLocal[vindex_1].position - vertexListLocal[vindex_0].position;
		u.w = 1.0f;

		Vector4 v = vertexListLocal[vindex_2].position - vertexListLocal[vindex_0].position;
		v.w = 1.0f;

		// compute cross product
		Vector4 n = Vector4::crossProduct(u, v);

		// update vertex array to flag this polygon as a contributor
		polys_touch_vertex[vindex_0]++;
		polys_touch_vertex[vindex_1]++;
		polys_touch_vertex[vindex_2]++;

		// now accumulate the normal into the vertex normal itself
		// note, we do NOT normalize at this point since we want the length of the normal
		// to weight on the average, and since the length is in fact the area of the parallelogram
		// constructed by uxv, so we are taking the "influence" of the area into consideration

		vertexListLocal[vindex_0].normal += n;
		vertexListLocal[vindex_0].normal.w = 1.0f;

		vertexListLocal[vindex_1].normal += n;
		vertexListLocal[vindex_1].normal.w = 1.0f;

		vertexListLocal[vindex_2].normal += n;
		vertexListLocal[vindex_2].normal.w = 1.0f;

	} // end if needs vertex normals

	// now we are almost done, we have accumulated all the vertex normals, but need to average them
	for (int vertex = 0; vertex < vertexCount; vertex++)
	{
		// if this vertex has any contributors then it must need averaging, OR we could check
		// the shading hints flags, they should be one to one

		// test if this vertex has a normal and needs averaging
		if (polys_touch_vertex[vertex] >= 1)
		{
			vertexListLocal[vertex].normal.x /= polys_touch_vertex[vertex];
			vertexListLocal[vertex].normal.y /= polys_touch_vertex[vertex];
			vertexListLocal[vertex].normal.z /= polys_touch_vertex[vertex];

			vertexListLocal[vertex].normal.normalise();
		} // end if

	} // end for
}
