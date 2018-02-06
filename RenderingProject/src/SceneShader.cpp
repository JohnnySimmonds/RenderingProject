/*
 * SceneShader.cpp
 *
 *  Created on: Nov 17, 2015
 *      Author: acarocha
 */

#include "SceneShader.h"

static float PI = 3.14159265359;


SceneShader::SceneShader(): Shader()
{
	_programPlane = 0;
	_programMesh = 0;
	_programEdge = 0;
	_planeVertexArray = -1;
	_planeVertexArray = -1;
	_mvUniform = -1;
	_projUniform = -1;
	_zTranslation = 1.0;
	_aspectRatio = 1.0;
	_xRot = 0.0;
	_yRot = 0.0;
	lightPosition = glm::vec3(0.5, 0.5, 0.5);
	yellow = 0.5;
	blue = 0.3;
	alpha = 0.9;
	beta = 0.5;	
	difRed = 0.5;
	difGreen = 0.5;
	difBlue = 0.5;
	bunnyOrDragon = 0;
	gooch = 0.0;
	figChoice = 0.0;
	silR = 1.0;
	reflect = 0.0;
	wired = false;
	minAng = 0;
	maxAng = 0;
	mousX = 0.0;
	mousY = 0.0;
	s = 0.5;
	intCheck = false;
	intCount = 0;
	artistReset = false;
	checkMesh = false;
	imageCorrection = glm::vec3(0.0,0.0,0.0);
}




/*----------------------Adjacency List functions----------------------*/
/*Creates the list based on the number of vertices*/
void SceneShader::createList(int size)
{
	bool sorted = false;
	int v1, v2, v3;
	
	edgeList.resize(size);
	
	
	for(int i = 0; i < _mesh->faces.size(); i++) //get the vertices for each triangle, sort it, and add it to the adjacency list
	{
		 v1 = _mesh->faces[i][0];
		 v2 = _mesh->faces[i][1];
		 v3 = _mesh->faces[i][2];
		
		while(!sorted)
		{
			if(v1 < v2 && v2 < v3)
				sorted = true;
			if(v1 > v3)
			{
				std::swap(v1, v3);	
			}
			if(v1 > v2)
			{
				std::swap(v1,v2);
			}
			if(v2 > v3)
			{
				std::swap(v2,v3);
			}
			sorted = true;
		}
		
		addEdge(v1,v2);
		addEdge(v1,v3);
		addEdge(v2,v3);
		
		sorted = false;
			
	}

}

/* Checks to see if the triangle is a front or backfacing triangle*/
void SceneShader::setBits(glm::vec3 V)
{
	glm::vec3 N, N1;
	bool sorted = false;
	bool front;
	float n1, n2, n3;
	int v1, v2, v3;
	
	

	
	for(int i = 0; i < _mesh->faces.size(); i++) //get the vertices for each triangle, sort it, and check whether it is front or back facing
	{
		 v1 = _mesh->faces[i][0];
		 v2 = _mesh->faces[i][1];
		 v3 = _mesh->faces[i][2];
		
		while(!sorted)
		{
			if(v1 < v2 && v2 < v3)
				sorted = true;
			if(v1 > v3)
			{
				std::swap(v1, v3);	
			}
			if(v1 > v2)
			{
				std::swap(v1,v2);
			}
			if(v2 > v3)
			{
				std::swap(v2,v3);
			}
			sorted = true;
		}
	
		trimesh::vec3 triNorm = _mesh->trinorm(i);
		
		
		N = glm::vec3(triNorm[0], triNorm[1], triNorm[2]);
		
		
		/* N dot V > 0 its front facing
		 * N dot V < 0 its back facing
		 * N dot V = 0 polygon is perpedicular to the viewing direction*/
		float dot = glm::dot(N, V);
		if( dot > 0.0000001f)
		{
			front = true;
		}
		else if (dot < 0.0000001f)
		{
			front = false;
		}
		/*Update edges*/
		
		updateEdge(v1, v2, front, i);
		updateEdge(v1, v3, front, i);
		updateEdge(v2, v3, front, i);
		
		
		
		sorted = false;
			
	}
	
	checkEdges();
	

}
/*Checks to see if the edge is a silhouette edge or within a threshold*/
void SceneShader::checkEdges()
{
	int v2;
	for(int v1 = 0; v1 < edgeList.size(); v1++) //checks go through each v1 in the list
	{
		currEdge = edgeList[v1].nextNode; //skip first one as it has useless info
		while(currEdge != NULL)
		{
			v2 = currEdge->V;
			checkDihedral(currEdge->T1, currEdge->T2);
			
			//if(intFound == false)
			//{
			//	printf("--------------INT PLZ-----------------------\n");
				if(intCheck)
					checkIntersection();
		//	}
			checkSil(v1, v2);
			resetBits();
			
			currEdge = currEdge->nextNode;
		}
	}
	intCount = 0;
//	intFound = false;
}
/*resets the bits of an edge after it has been added to the edge buffer*/
void SceneShader::resetBits()
{
	if(artistReset)
		currEdge->A = 0;
	
	currEdge->F = 0;
	currEdge->B = 0;
	currEdge->Fa = 0;
	currEdge->Ba = 0;
	currEdge->T1 = 0;
	currEdge->T2 = 0;
	currEdge->t1Set = false;
	
	
}
/*updates the bits on an edge*/
void SceneShader::updateEdge(int v1, int v2, bool front, int triNum)
{
	
	currEdge = &edgeList[v1];
	if(currEdge == NULL)
		return;
	while(currEdge->nextNode != NULL && currEdge->V != v2)
		currEdge = currEdge->nextNode;

	if(currEdge->V == v2)
	{
		if(!currEdge->t1Set)
		{
			currEdge->T1 = triNum;
			currEdge->t1Set = true;
		}
		else
		{
			currEdge->T2 = triNum;
		}
		if(front)
		{
			
			currEdge->F = currEdge->F ^ 1;
			currEdge->Fa = currEdge->Fa || 1;
			
		}
		else
		{
			currEdge->B = currEdge->B ^ 1;
			currEdge->Ba = currEdge->Ba || 1;
			
		}
	}
	
}
/* Add edge to the Adjacency list*/
void SceneShader::addEdge(int v1, int v2)
{
	
		currEdge = &edgeList[v1];
	
		while(currEdge->nextNode != NULL && currEdge->V != v2) //get to the last node for what vertex you are on
			currEdge = currEdge->nextNode;
		if(currEdge != NULL && currEdge->V != v2)
		{
			newEdge = new edgeDetails();
			newEdge->V = v2;
			currEdge->nextNode = newEdge;
		}
	
}
/*Extra function for error checking*/
void SceneShader::printList()
{
	for(int i = 0; i < edgeList.size(); i++)
	{
		currEdge = edgeList[i].nextNode; //skip the first place holder edge
		while(currEdge != NULL)
		{
			printf("V1: %d, V2: %d, T1: %d, T2: %d\n", i,  currEdge->V, currEdge->T1, currEdge-> T2);
			currEdge = currEdge->nextNode;
		}
		
	}
}
void SceneShader::deleteList()
{
	edgeDetails* toDel;
	for(int i = 0; i < edgeList.size(); i++)
	{
		currEdge = edgeList[i].nextNode; //skip the first place holder edge
		while(currEdge != NULL)
		{
			//printf("V1: %d, V2: %d, T1: %d, T2: %d\n", i,  currEdge->V, currEdge->T1, currEdge-> T2);
			toDel = currEdge;
			currEdge = currEdge->nextNode;
			delete(toDel);
		}
		
	}
}
/*Initializes the edge buffer*/
void SceneShader:: createEdgeBuffer()
{
	
	glBindVertexArray(_meshVertexArray);

	glGenBuffers(1, &_edgeBuffer);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _edgeBuffer);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER,  edgeBuffer.size() * sizeof(unsigned int), edgeBuffer.data(), GL_DYNAMIC_DRAW);

	glBindVertexArray(0);
	
	
}
void SceneShader::checkDihedral(int t1, int t2)
{
	trimesh::vec3 triNorm1 = _mesh->trinorm(t1);
	trimesh::vec3 triNorm2 = _mesh->trinorm(t2);
	
	glm::vec3 tri1(triNorm1[0], triNorm1[1], triNorm1[2]);
	glm::vec3 tri2(triNorm2[0], triNorm2[1], triNorm2[2]);
	
	tri1 = normalize(tri1);
	tri2 = normalize(tri2);
	float dAng = acos(glm::dot(tri1, tri2));
	int angDegree = (int)((dAng/2.0f * 180.0f)/ PI);
	if((minAng <= angDegree && maxAng >= angDegree))
	{
		currEdge->A = 1;
	}
	
}
/* TODO right now trying to figure out how to get the correct position for the triangle that im clicking on/ generating a ray from the mouse position that I click on*/
void SceneShader::checkIntersection()
{
	glm::vec3 eye(0.0f, 0.3f, 2.0f);
	glm::vec3 up(0.0f, 1.0f, 0.0f);
	glm::vec3 center(0.0f, 0.0f, 0.0f);
	glm::mat4 identity(1.0f);
	
	_modelview = lookAt(eye, center, up);
	
	glm::mat4 rotationX = glm::rotate(identity, _yRot  * PI/180.0f, glm::vec3(0.0f, 1.0f, 0.0f));
	glm::mat4 rotationY = glm::rotate(identity, _xRot  * PI/180.0f, glm::vec3(1.0f, 0.0f, 0.0f));
	glm::mat4 translateZ = glm::translate(identity, glm::vec3(0.0f, 0.0f ,_zTranslation));
	glm::mat4 scaleMat = glm::scale(glm::vec3(s, s, s));
	_modelview *= translateZ * rotationY * rotationX * scaleMat;
	glm::mat4 viewModel = glm::inverse(_modelview);
	glm::vec3 origin(-viewModel[3][0], -viewModel[3][1], -viewModel[3][2]);
	
	bool mouseInt = false;
	int t1 = currEdge->T1;
	int t2 = currEdge->T2;
	/* convert to object space*/
	glm::vec3 mousRay = genRay();
	
	mousRay = normalize(mousRay) - origin;
	mouseInt = rayTriInt(mousRay, t1);

	if(mouseInt)
	{
		currEdge->A = 1;
		
		//if(intCount == 6)
	//		intFound = true;
	
	//	intCount++;
		//printf("intersections checked: %d\n", intCount);
	}
	else
	{
		mouseInt = rayTriInt(mousRay, t2);
		if(mouseInt)
		{
			currEdge->A = 1;
			
		//	if(intCount == 6)
		//		intFound = true;
			
		//	intCount++;
		//	printf("intersections checked: %d\n", intCount);
		}
	}
	
}

bool SceneShader::rayTriInt(glm::vec3 mousRay, int Tri)
{
	
	glm::vec3 eye(0.0f, 0.3f, 2.0f);
	glm::vec3 up(0.0f, 1.0f, 0.0f);
	glm::vec3 center(0.0f, 0.0f, 0.0f);
	glm::mat4 identity(1.0f);
	
	int v1 = _mesh->faces[Tri][0];
	int v2 = _mesh->faces[Tri][1];
	int v3 = _mesh->faces[Tri][2];
	
	/* getting the vec3 vertices of the triangle*/
	glm::vec3 p1(_mesh->vertices[v1][0], _mesh->vertices[v1][1], _mesh->vertices[v1][2]);
	glm::vec3 p2(_mesh->vertices[v2][0], _mesh->vertices[v2][1], _mesh->vertices[v2][2]);
	glm::vec3 p3(_mesh->vertices[v3][0], _mesh->vertices[v3][1], _mesh->vertices[v3][2]);
	
	_modelview = lookAt(eye, center, up);
	
	_projection = glm::perspective( 45.0f, _aspectRatio, 0.01f, 100.0f);
	glm::mat4 rotationX = glm::rotate(identity, _yRot  * PI/180.0f, glm::vec3(0.0f, 1.0f, 0.0f));
	glm::mat4 rotationY = glm::rotate(identity, _xRot  * PI/180.0f, glm::vec3(1.0f, 0.0f, 0.0f));
	glm::mat4 translateZ = glm::translate(identity, glm::vec3(0.0f, 0.0f ,_zTranslation));
	glm::mat4 scaleMat = glm::scale(glm::vec3(s, s, s));
	_modelview *= translateZ * rotationY * rotationX * scaleMat;

	p1 = p1 + imageCorrection;
	p2 = p2 + imageCorrection;
	p3 = p3 + imageCorrection;


	glm::vec4 P1 = _projection * _modelview * glm::vec4(p1, 1.0);
	glm::vec4 P2 = _projection * _modelview * glm::vec4(p2, 1.0);
	glm::vec4 P3 = _projection * _modelview * glm::vec4(p3, 1.0);

	
	//p1 = glm::vec3(P1.x/P1.w, P1.y/P1.w, P1.z/P1.w);
//	p2 = glm::vec3(P2.x/P2.w, P2.y/P2.w, P2.z/P2.w);
//	p3 = glm::vec3(P3.x/P3.w, P3.y/P3.w, P3.z/P3.w);
	
	
	p1 = normalize(glm::vec3(P1.x, P1.y, P1.z));
	p2 = normalize(glm::vec3(P2.x, P2.y, P2.z));
	p3 = normalize(glm::vec3(P3.x, P3.y, P3.z));
	
	
	/* RAY TRIANGLE INTERSECTION*/
	
	glm::vec3 P, Q, T;
	glm::vec3 e1, e2;
	double det, u, v, inv_det;
	double x,y,z;
	double t;
	double uv;
	bool CULLING = false;//used for only checking front facing triangles
	float EPSILON = 0.000001;
	glm::mat4 viewModel = glm::inverse(_modelview);
	glm::vec3 origin(viewModel[3][0], viewModel[3][1], viewModel[3][2]);
	origin = -origin;
	
	e1 = p2 - p1;
	e2 = p3 - p1;
	
	P = glm::cross(mousRay, e2);
	
	det = glm::dot(e1, P);
	
	if(CULLING)
	{
		if(det < EPSILON)
			return false;
			
		T = origin - p1;
		u = glm::dot(T, P);
		if( u < 0.0 || u > det)
			return false;
		
		Q = glm::cross(T, e1);
		
		v = glm::dot(mousRay, Q);
		
		if(v < 0.0 || u + v > det)
			return false;
		
	}
	else
	{
		if( det > -EPSILON && det < EPSILON)
			return 0;
			
		inv_det = 1.0/det;
		
		T = origin - p1;
		u = glm::dot(T, P) * inv_det;
		
		if(u < 0.0 || u > 1.0)
			return false;
			
		Q = glm::cross(T, e1);
		
		v = glm::dot(mousRay, Q) * inv_det;
		
		if(v < 0.0 || u + v > 1.0)
			return false;
		
	}
	return true;

}
/* for mouse coord triangle intersection*/
glm::vec3 SceneShader::genRay()
{
	
	double w = 1024.0;
	double h = 1024.0;

	float fovx = PI/4.0;
	float fovy = (h/w) * fovx;

	double vx = ((2*mousX-w)/w) * tan(fovx);
	double vy = -((2*mousY-h)/h) * tan(fovy);

	glm::vec3 ray(vx, vy, 1.0);
	
	return ray;

}
void SceneShader::checkSil(int v1, int v2)
{
	
	if((currEdge->F == 1 && currEdge->B == 1)) //silhouette edge
	{
		edgeBuffer.push_back(v1);
		edgeBuffer.push_back(v2);
	}
	else if(currEdge->A == 1) // user drawn edges
	{
		edgeBuffer.push_back(v1);
		edgeBuffer.push_back(v2);
	}
	else if(currEdge->F == 1 && currEdge->B == 0 && currEdge->Fa == 1 && currEdge->Ba == 0)//front boundry edge
	{
		edgeBuffer.push_back(v1);
		edgeBuffer.push_back(v2);
	}
	else if(currEdge->F == 0 && currEdge->B == 1 && currEdge->Fa == 0 && currEdge->Ba == 1) //back boundry edge
	{
		edgeBuffer.push_back(v1);
		edgeBuffer.push_back(v2);
	}
}

/*--------------------------------------------------------------------*/

void SceneShader::calculateCylindricalUVCoordinates()
{
	/* calculate UV*/
	for(unsigned int i = 0; i < _mesh->vertices.size(); i++)
	{
		glm::vec3 vertex(_mesh->vertices[i][0],
		_mesh->vertices[i][1],
		_mesh->vertices[i][2]);
		
		float theta = glm::atan(vertex.z,vertex.x)/PI;
		float h = vertex.y * 4.0;
		
		_uvs.push_back(glm::vec2(theta,h));
		
	}
	
}

void SceneShader::readMesh( std::string filename )
{
	_mesh = trimesh::TriMesh::read(filename);

	_mesh->need_bbox();
	_mesh->need_faces();
	_mesh->need_normals();
	_mesh->need_bsphere();

	
	for(unsigned int i = 0; i < _mesh->faces.size(); i++)
	{
		_triangleIndices.push_back(_mesh->faces[i][0]);
		_triangleIndices.push_back(_mesh->faces[i][1]);
		_triangleIndices.push_back(_mesh->faces[i][2]);
	}
}

void SceneShader::createVertexBuffer()
{
	
		//1D Texture
	std::vector<GLubyte> colormap;
	colormap.push_back(255);//r
	colormap.push_back(0);//g
	colormap.push_back(0);//b
	colormap.push_back(255);//a
	
	colormap.push_back(0);//r
	colormap.push_back(0);//g
	colormap.push_back(255);//b
	colormap.push_back(255);//a
	
		static const GLubyte colorMap[]
	{
		247,252,253,
		224,236,244,
		191,211,230,
		158,188,218,
		140,150,198,
		140,107,177,
		136,65,157,
		129,15,124,
		77,0,75
	};
	
	_textureID = _texture.create1DTexture(colormap);
	
	//create plane geometry
	static const GLfloat quadData[] =
	{
                        -1.0f, 0.0f, -1.0f,
                        -1.0f, 0.0f, 1.0f,
                        1.0f, 0.0f, -1.0f,
                        1.0f, 0.0f, 1.0f,
	};

	static const GLfloat uvsPlane[] =
	{
                        0.0f, 0.0f,
                        0.0f, 1.0f,
                        1.0f, 0.0f,
                        1.0f, 1.0f
	};



	//passing model attributes to the GPU
	//plane
	glGenVertexArrays(1, &_planeVertexArray);
	glBindVertexArray(_planeVertexArray);

	glGenBuffers(1, &_planeVertexBuffer);
	glBindBuffer(GL_ARRAY_BUFFER, _planeVertexBuffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof (quadData), quadData, GL_STATIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, NULL);
	glEnableVertexAttribArray(0);

	glGenBuffers(1, &_planeTextureBuffer);
	glBindBuffer(GL_ARRAY_BUFFER, _planeTextureBuffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof (uvsPlane), uvsPlane, GL_STATIC_DRAW);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 0, NULL);
	glEnableVertexAttribArray(1);


	glBindVertexArray(0);

	//read and create mesh geometry
	//TODO add ability to switch between dragon and bunny
	
	if(bunnyOrDragon == 1.0)
	{
		readMesh("./models/dragon.ply"); //normalized vertices coordinates
		imageCorrection = glm::vec3(0.0,0.0,0.0);
	}
	else if (bunnyOrDragon == 2.0)
	{
		readMesh("./models/bunny.ply");
		imageCorrection = glm::vec3 (0.0, -0.12, 0.0);
	}
	else if (bunnyOrDragon == 3.0)
	{
		readMesh("./models/happy.ply");
		imageCorrection = glm::vec3(0.0,-0.1,0.0);
	}
	else if (bunnyOrDragon == 4.0)
	{
		readMesh("./models/cube.ply");
		imageCorrection = glm::vec3(0.0,0.0,0.0);
	}
	else if (bunnyOrDragon == 5.0)
	{
		readMesh("./models/dolphins.ply");
		imageCorrection = glm::vec3(-20.5814, 52.2077, 0.0);
	}
	else if (bunnyOrDragon == 6.0)
	{
		readMesh("./models/Raindeer.ply");
		imageCorrection = glm::vec3(0.463005, -22.5763, 0);
	}
	else if (bunnyOrDragon == 7.0)
	{
		readMesh("./models/openBox.ply");
		imageCorrection = glm::vec3(0.0,0.0,0.0);
	}

	std::string fileName;
	if(figChoice == 1.0)
		fileName = "textures/fig-7b.png";
	if(figChoice == 2.0)
		fileName = "textures/fig-7c.png";
	if(figChoice == 3.0)
		 fileName = "textures/fig-7d.png";
	if(figChoice == 4.0)
		fileName = "textures/fig-10b.png";
	if(figChoice == 5.0)
		fileName = "textures/fig-10c.png";
	if(figChoice == 6.0)
		fileName = "textures/fig-10d.png";
	if(figChoice == 7.0)
		fileName =  "textures/fig-11b.png";
	if(figChoice == 8.0)
		fileName = "textures/fig-11c.png";
	if(figChoice == 9.0)
		fileName = "textures/fig-11d.png";
	if(figChoice == 10.0)
		fileName = "textures/fig-8a.png";
	if(figChoice == 11.0)
		fileName = "textures/fig-8b.png";
	if(figChoice == 12.0)
		fileName =  "textures/fig-9b.png";
	if(figChoice == 13.0)
		fileName = "textures/fig-9c.png";
	if(figChoice == 14.0)
		fileName = "textures/fig-9e.png";
	if(figChoice == 15.0)
		fileName = "textures/fig-9f.png";
	
	std::string imageFilename(fileName);
	unsigned int error = lodepng::decode(_image, imageWidth, imageHeight, imageFilename.c_str());
	if(error) 
		{
			std::cout << "error..." << error << ": " << lodepng_error_text(error) << std::endl; 
		}
	
	//creating 2D texture
	_texture2Did = _texture.create2DTexture(_image, imageWidth, imageHeight);
	
	calculateCylindricalUVCoordinates();
	
	//triangle mesh
	glGenVertexArrays(1, &_meshVertexArray);
	glBindVertexArray(_meshVertexArray);

	glGenBuffers(1, &_meshVertexBuffer);
	glBindBuffer(GL_ARRAY_BUFFER, _meshVertexBuffer);
	glBufferData(GL_ARRAY_BUFFER,  _mesh->vertices.size() * sizeof (trimesh::point), _mesh->vertices.data(), GL_STATIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, NULL);
	glEnableVertexAttribArray(0);
	
	
	
	glGenBuffers(1, &_meshNormalBuffer);
	glBindBuffer(GL_ARRAY_BUFFER, _meshNormalBuffer);
	glBufferData(GL_ARRAY_BUFFER,  _mesh->normals.size() * sizeof (trimesh::vec), _mesh->normals.data(), GL_STATIC_DRAW);
	glBufferData(GL_ARRAY_BUFFER,  _mesh->normals.size() * sizeof (trimesh::vec), _mesh->normals.data(), GL_STATIC_DRAW);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, NULL);
	glEnableVertexAttribArray(1);
	
	glGenBuffers(1, &_meshTextureBuffer);
	glBindBuffer(GL_ARRAY_BUFFER, _meshTextureBuffer);
	glBufferData(GL_ARRAY_BUFFER,  _uvs.size()*sizeof(glm::vec2), _uvs.data(), GL_STATIC_DRAW);
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 0, NULL);
	glEnableVertexAttribArray(2);

	glGenBuffers(1, &_meshIndicesBuffer );
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _meshIndicesBuffer );
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, _triangleIndices.size()*sizeof(unsigned int), _triangleIndices.data(), GL_STATIC_DRAW);



	glBindVertexArray(0);

}


void SceneShader::startup(int choice, int figChoiceMain, float r, int min, int max, float scale)
{
	bunnyOrDragon = choice;
	figChoice = figChoiceMain;
	reflect = r;
	minAng = min;
	maxAng = max;
	s = scale;
	//s = 0.01;
	_programPlane = compile_shaders("./shaders/plane.vert", "./shaders/plane.frag");

	_programMesh = compile_shaders("./shaders/mesh.vert", "./shaders/mesh.frag");

	_programLight = compile_shaders("./shaders/light.vert", "./shaders/light.frag");
	
	_programEdge = compile_shaders("./shaders/edge.vert", "./shaders/edge.frag");

	createVertexBuffer();
	
	//Create everything for the edgeBuffer
	createList(_mesh->vertices.size());
	createEdgeBuffer();
	
}

void SceneShader::renderPlane()
{
	glBindVertexArray(_planeVertexArray);

	glUseProgram(_programPlane);
	_texture.bind1DTexture(_programPlane, _textureID, std:: string("colormap"));
	//scene matrices and camera setup
	glm::vec3 eye(0.0f, 0.3f, 2.0f);
	glm::vec3 up(0.0f, 1.0f, 0.0f);
	glm::vec3 center(0.0f, 0.0f, 0.0f);

	_modelview = glm::lookAt( eye, center, up);

	glm::mat4 identity(1.0f);
	_projection = glm::perspective( 45.0f, _aspectRatio, 0.01f, 100.0f);

	glm::mat4 rotationX = glm::rotate(identity, _yRot  * PI/180.0f, glm::vec3(0.0f, 1.0f, 0.0f));

   _modelview *=  rotationX;
	
	
	
	//Uniform variables
	glUniformMatrix4fv(glGetUniformLocation(_programPlane, "modelview"), 1, GL_FALSE, glm::value_ptr(_modelview));
	glUniformMatrix4fv(glGetUniformLocation(_programPlane, "projection"), 1, GL_FALSE, glm::value_ptr(_projection));

	glUniform3fv(glGetUniformLocation(_programPlane, "lightPosition"), 1, glm::value_ptr(lightPosition) );

	glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
	
	_texture.unbind1DTexture();

	glBindVertexArray(0);
}

void SceneShader::renderMesh()
{
	glBindVertexArray(_meshVertexArray);

	glUseProgram(_programMesh);
	_texture.bind2DTexture(_programMesh, _texture2Did, std::string("image"));
	//scene matrices and camera setup
	glm::vec3 eye(0.0f, 0.3f, 2.0f);
	glm::vec3 up(0.0f, 1.0f, 0.0f);
	glm::vec3 center(0.0f, 0.0f, 0.0f);


	_modelview = glm::lookAt(eye, center, up);

	_projection = glm::perspective( 45.0f, _aspectRatio, 0.01f, 100.0f);

	glm::mat4 identity(1.0f);

	glm::mat4 rotationX = glm::rotate(identity, _yRot  * PI/180.0f, glm::vec3(0.0f, 1.0f, 0.0f));
	
	
	/*For rotationY is for rotating the object in every direction*/
	glm::mat4 rotationY = glm::rotate(identity, _xRot  * PI/180.0f, glm::vec3(1.0f, 0.0f, 0.0f));
	
	glm::mat4 translateZ = glm::translate(identity, glm::vec3(0.0f, 0.0f ,_zTranslation));
	glm::mat4 scaleMat = glm::scale(glm::vec3(s, s, s));
	_modelview *= translateZ * rotationY * rotationX * scaleMat;
  
   


	//uniform variables
	glUniformMatrix4fv(glGetUniformLocation(_programMesh, "modelview"), 1, GL_FALSE, glm::value_ptr(_modelview));
	glUniformMatrix4fv(glGetUniformLocation(_programMesh, "projection"), 1, GL_FALSE, glm::value_ptr(_projection));


	
	glUniform3fv(glGetUniformLocation(_programMesh, "lightPosition"), 1, glm::value_ptr(lightPosition) );
	glUniform1f(glGetUniformLocation(_programMesh, "yellow"), yellow);
	glUniform1f(glGetUniformLocation(_programMesh, "blue"), blue);
	glUniform1f(glGetUniformLocation(_programMesh, "alpha"), alpha);
	glUniform1f(glGetUniformLocation(_programMesh, "beta"), beta);
	glUniform1f(glGetUniformLocation(_programMesh, "difRed"), difRed);
	glUniform1f(glGetUniformLocation(_programMesh, "difBlue"), difBlue);
	glUniform1f(glGetUniformLocation(_programMesh, "difGreen"), difGreen);
	glUniform1f(glGetUniformLocation(_programMesh, "bunnyOrDragon"), bunnyOrDragon);
	glUniform1f(glGetUniformLocation(_programMesh, "gooch"), gooch);
	glUniform1f(glGetUniformLocation(_programMesh, "silR"), silR);
	glUniform1f(glGetUniformLocation(_programMesh, "reflectCheck"), reflect);

	if(wired)
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	else
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _meshIndicesBuffer );
	glDrawElements(GL_TRIANGLES, _mesh->faces.size()*3, GL_UNSIGNED_INT, 0);
	
	_texture.unbind2DTexture();
	glBindVertexArray(0);
	glUseProgram(0);
}


void SceneShader::renderLight()
{
	glUseProgram(_programLight);

	//scene matrices and camera setup
	glm::vec3 eye(-5.0f, 0.3f, 2.0f);
	glm::vec3 up(0.0f, 1.0f, 0.0f);
	glm::vec3 center(0.0f, 0.0f, 0.0f);

	_modelview = glm::lookAt( eye, center, up);

	_projection = glm::perspective( 45.0f, _aspectRatio, 0.01f, 100.0f);

	glm::mat4 identity(1.0f);

	glm::mat4 rotationX = glm::rotate(identity, _yRot  * PI/180.0f, glm::vec3(0.0f, 1.0f, 0.0f));
	glm::mat4 rotationY = glm::rotate(identity, _xRot  * PI/180.0f, glm::vec3(1.0f, 0.0f, 0.0f));
	glm::mat4 translateZ = glm::translate(identity, glm::vec3(0.0f, 0.0f ,_zTranslation));
	
   _modelview *=  translateZ * rotationY * rotationX;

	//uniform variables
	glUniformMatrix4fv(glGetUniformLocation(_programLight, "modelview"), 1, GL_FALSE, glm::value_ptr(_modelview));
	glUniformMatrix4fv(glGetUniformLocation(_programLight, "projection"), 1, GL_FALSE, glm::value_ptr(_projection));

	glUniform3fv(glGetUniformLocation(_programLight, "lightPosition"), 1, glm::value_ptr(lightPosition) );

	glPointSize(30.0f);
	glDrawArrays( GL_POINTS, 0, 1);
	glUseProgram(0);
}

void SceneShader::renderEdge()
{
	glBindVertexArray(_meshVertexArray);
	glUseProgram(_programEdge);
	
	//scene matrices and camera setup
	glm::vec3 eye(0.0f, 0.3f, 2.0f);
	glm::vec3 up(0.0f, 1.0f, 0.0f);
	glm::vec3 center(0.0f, 0.0f, 0.0f);

	_modelview = glm::lookAt(eye, center, up);

	glm::mat4 identity(1.0f);
	_projection = glm::perspective( 45.0f, _aspectRatio, 0.01f, 100.0f);

	glm::mat4 rotationX = glm::rotate(identity, _yRot  * PI/180.0f, glm::vec3(0.0f, 1.0f, 0.0f));
	glm::mat4 rotationY = glm::rotate(identity, _xRot  * PI/180.0f, glm::vec3(1.0f, 0.0f, 0.0f));
	glm::mat4 translateZ = glm::translate(identity, glm::vec3(0.0f, 0.0f ,_zTranslation));
	glm::mat4 scaleMat = glm::scale(glm::vec3(s, s, s));
	_modelview *= translateZ * rotationY * rotationX * scaleMat;

	

	glm::mat4 viewModel = glm::inverse(_modelview);
	glm::vec3 V(viewModel[3][0], viewModel[3][1], viewModel[3][2]); //dont know if the z value is 1 or not so have to divide by eye.w
	
 
	
	setBits(-V);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _edgeBuffer);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER,  edgeBuffer.size() * sizeof(unsigned int), edgeBuffer.data(), GL_DYNAMIC_DRAW);
	
	
	
	//Uniform variables
	glUniformMatrix4fv(glGetUniformLocation(_programEdge, "modelview"), 1, GL_FALSE, glm::value_ptr(_modelview));
	glUniformMatrix4fv(glGetUniformLocation(_programEdge, "projection"), 1, GL_FALSE, glm::value_ptr(_projection));

	glUniform3fv(glGetUniformLocation(_programEdge, "lightPosition"), 1, glm::value_ptr(lightPosition) );
	

	glUniform1f(glGetUniformLocation(_programEdge, "bunnyOrDragon"), bunnyOrDragon);
	
	glLineWidth(5.0);
	
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _edgeBuffer);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER,  edgeBuffer.size() * sizeof(unsigned int), edgeBuffer.data(), GL_DYNAMIC_DRAW);
	
	glDrawElements(GL_LINES, edgeBuffer.size(), GL_UNSIGNED_INT, 0);
	edgeBuffer.clear();
	
	glBindVertexArray(0);
	glUseProgram(0);
}

void SceneShader::render()
{
	
	//renderPlane();
	if(checkMesh)
		renderMesh();
	renderLight();
	
	renderEdge();
	
	artistReset = false;

}

void SceneShader::setZTranslation(float z)
{
	_zTranslation = z;
}

void SceneShader::setAspectRatio(float ratio)
{
	_aspectRatio = ratio;
}

void SceneShader::setRotationX( float x )
{
	_xRot = x;
}

void SceneShader::setRotationY( float y )
{
	_yRot = y;
}

void SceneShader::shutdown()
{
	
	glDeleteBuffers(1, &_meshVertexBuffer);
	glDeleteBuffers(1, &_meshNormalBuffer);
	glDeleteBuffers(1, &_meshIndicesBuffer );
	glDeleteBuffers(1, &_edgeBuffer);
	glDeleteVertexArrays(1, &_meshVertexArray);
	glDeleteVertexArrays(1, &_planeVertexArray);
	deleteList();

}

void SceneShader::updateLightPositionX(float x)
{
	lightPosition.x += x;
}

void SceneShader::updateLightPositionY(float y)
{
	lightPosition.y += y;
}

void SceneShader::updateLightPositionZ(float z)
{
	lightPosition.z += z;
}
void SceneShader::updateYellow(float y)
{
		yellow += y;
		if(yellow > 1 || yellow < 0)
		{
			yellow -= y;
		}
}
void SceneShader::updateBlue(float b)
{
		blue += b;	
		if(blue > 1 || blue < 0)
		{
			blue -= b;
		}
}

void SceneShader::updateAlpha(float a)
{
		alpha += a;
		if(alpha > 1 || alpha < 0)
		{
			alpha -= a;
		}
}
void SceneShader::updateBeta(float be)
{
		beta += be;
		if(beta > 1 || beta < 0)
		{
			beta -= be;
		}
}

void SceneShader::updateDifBlue(float blu)
{
		difBlue += blu;
		if(difBlue > 1 || difBlue < 0)
		{
			difBlue -= blu;
		}
}void SceneShader::updateDifGreen(float g)
{
		difGreen += g;
		if(difGreen > 1 || difGreen < 0)
		{
			difGreen -= g;
		}
}void SceneShader::updateDifRed(float r)
{
		difRed += r;
		if(difRed > 1 || difRed < 0)
		{
			difRed -= r;
		}
}void SceneShader::updateImage(float choice)
{
		bunnyOrDragon = choice;
			
}
void SceneShader::updateGooch(float onOff)
{
	gooch = onOff;
}
void SceneShader::clearScreen()
{
	glClearColor(0.2, 0.2, 0.2, 1.0);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
   // _mesh->faces.clear();
}
void SceneShader::createVertex()
{
	createVertexBuffer();
}
void SceneShader::updateSilR(float r)
{
	silR += r;
	if(silR < 0)
		silR -= r;
}
void SceneShader::updateReflect(float r)
{
 reflect = r;	
}

void SceneShader::updateWired(bool w)
{
	wired = w;
}
void SceneShader::updateIntCheck(bool check)
{
	intCheck = check;
}
void SceneShader::updateArtist(bool reset)
{
	artistReset = reset;
	
}
void SceneShader::updateMesh(bool checkMeshMain)
{
	checkMesh = checkMeshMain;
}

void SceneShader::setMousePos(float x, float y)
{
	mousX = x;
	mousY = y;
}
SceneShader::~SceneShader()
{
	shutdown();
}
