#pragma once
#include "include/gl/glaux.h"

class CModel
{
	public:
		struct Mesh
		{
			int m_materialIndex;
			int m_numTriangles;
			int *m_pTriangleIndices;
		};
		//	Material properties
		struct Material
		{
			float m_ambient[4], m_diffuse[4], m_specular[4], m_emissive[4];
			float m_shininess;
			GLuint m_texture;
			char *m_pTextureFilename;
		};

		//	Triangle structure
		struct Triangle
		{
			float m_vertexNormals[3][3];
			float m_s[3], m_t[3];
			int m_vertexIndices[3];
		};

		//	Vertex structure
		struct Vertex
		{
			char m_boneID;	// for skeletal animation
			float m_location[3];
		};

	public:
		CModel();
		virtual ~CModel();

		//Load the model data into the private variables. 
		virtual bool loadModelData( const char *filename ) = 0;

		//Draw the model.		
		void draw();
		
		//Called if OpenGL context was lost and we need to reload textures, display lists, etc.
		void reloadTextures();

		GLuint LoadGLTex(  char *filename );
		AUX_RGBImageRec* LoadBMP(char *Filename);
	protected:
		//	Meshes used
		int m_numMeshes;
		Mesh *m_pMeshes;

		//	Materials used
		int m_numMaterials;
		Material *m_pMaterials;

		//	Triangles used
		int m_numTriangles;
		Triangle *m_pTriangles;

		//	Vertices Used
		int m_numVertices;
		Vertex *m_pVertices;
		GLuint m_nTexturem;
};

class MilkshapeModel : public CModel
{
	public:
		MilkshapeModel();
		virtual ~MilkshapeModel();
	
		//Load the model data into the private variables. 		
		virtual bool loadModelData( const char *filename );
};
