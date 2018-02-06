/*
 * SceneShader.h
 *
 *  Created on: Nov 17, 2015
 *      Author: acarocha
 */

#ifndef SCENESHADER_H_
#define SCENESHADER_H_

#include "Shader.h"
#include <vector>

#define GLM_FORCE_RADIANS

#include "TriMesh.h"
#include "texture.h"
#include "lodepng.h"
#include <glm/glm.hpp>
#include <glm/gtx/transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include "GL/glut.h" 


class SceneShader : public Shader
{
public:

	SceneShader();
	~SceneShader();

	void startup(int choice, int figChoiceMain, float r, int min, int max, float scale);
	void shutdown ();
	void render();

	void renderPlane();
	void renderMesh();
	void renderLight();

	void setAspectRatio( float ratio );
	void setZTranslation(float z);
	void setRotationX( float x );
	void setRotationY( float y );

	void calculateCylindricalUVCoordinates();

	void updateLightPositionX( float x );
	void updateLightPositionY( float y );
	void updateLightPositionZ( float z );
	void updateYellow(float y);
	void updateBlue(float b);
	void updateBeta(float be);
	void updateAlpha(float a);
	void updateDifBlue(float blu);
	void updateDifGreen(float g);
	void updateDifRed(float r);
	void updateImage(float choice);
	void updateGooch(float onOff);
	void clearScreen();
	void createVertex();
	void updateSilR(float r);
	void updateReflect(float r);
	void updateWired(bool w);
	void createList(int size);
	void addEdge(int v1, int v2);
	void setBits(glm::vec3 V);
	void updateEdge(int v1, int v2, bool front, int triNum);
	void createEdgeBuffer();
	void checkSil(int v1, int v2);
	void renderEdge();
	void printList();
	void checkDihedral(int t1, int t2);
	void checkEdges();
	void resetBits();
	void checkIntersection();
	void setMousePos(float x, float y);
	glm::vec3 genRay();
	bool rayTriInt(glm::vec3 mousRay, int Tri);
	void deleteList();
	void updateIntCheck(bool check);
	void updateArtist(bool reset);
	void updateMesh(bool checkMeshMain);
	
private:

	/*methods*/

	void readMesh( std::string filename );
	void createVertexBuffer();


	/*variables*/
	GLuint _programLight;
	GLuint _programPlane;
	GLuint _programMesh;
	GLuint _programEdge;

	GLuint _planeVertexArray;
	GLuint _planeVertexBuffer;
	GLuint _planeTextureBuffer;
	
	GLuint _edgeBuffer;

	GLuint _meshVertexArray;
	GLuint _meshVertexBuffer;
	GLuint _meshNormalBuffer;
	GLuint _meshIndicesBuffer;
	GLuint _meshTextureBuffer;

    GLint _mvUniform, _projUniform;

	std::vector<unsigned char> _image;
	unsigned int imageWidth;
	unsigned int imageHeight;

	/* Matrices */
	glm::mat4 _modelview;
	glm::mat4 _projection;

	float _zTranslation;
	float _xRot;
	float _yRot;
	float _aspectRatio;

	/* Textures */
	GLuint _textureID;
	Texture _texture;
	
	GLuint _texture2Did;

	trimesh::TriMesh* _mesh;
	std::vector<unsigned int> _triangleIndices;

	std::vector<glm::vec2> _uvs;
	
	glm::vec3 imageCorrection;
	glm::vec3 lightPosition;
	float yellow;
	float alpha;
	float beta;
	float blue;
	float difRed;
	float difBlue;
	float difGreen;
	float choice;
	int bunnyOrDragon;
	float gooch;
	int figChoice;
	float silR;
	float reflect;
	bool wired;
	int maxAng;
	int minAng;
	glm::vec3 lastFrameV;
	float mousX;
	float mousY;
	float s;
	bool intCheck;
	int intCount;
	bool artistReset;
	bool checkMesh;
	
	/* Edgebuffer stuff*/
	std::vector<unsigned int> edgeBuffer;
	//std::vector<vec3> edgeBuffer;
	/*Adjaceny List variables*/
	
	struct edgeDetails
{
    // OpenGL names for array buffer objects, vertex array object
    GLuint  V;  //vertex
    GLuint  A; //artist bit
    GLuint  F;	//front face
    GLuint  B;	//back face
    GLuint  Fa;	//front absolute
    GLuint  Ba;	//back absolute
    GLuint  T1;	//Triangle 1
    GLuint  T2;	//Triangle 2
    edgeDetails* nextNode;
    bool t1Set;
    // initialize object names to zero (OpenGL reserved value)
    edgeDetails() : V(0), A(0), F(0), B(0), Fa(0), Ba(0), T1(0), T2(0), nextNode(NULL), t1Set(false) {}
};
	
	edgeDetails* newEdge;
	std::vector<edgeDetails> edgeList;
	edgeDetails* currEdge;

};

#endif /* SCENESHADER_H_ */
