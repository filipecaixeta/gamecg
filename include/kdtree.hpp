#ifndef	KDTREE_H
#define KDTREE_H
#include <mesh.h>
#include <glm/glm.hpp>
#include <vector>
using glm::vec3;

struct kdNode
{
	Mesh *mesh;
	kdNode *left, *right;
};

class kdTree
{
public:
	kdTree():root(NULL){};
	void insert(Mesh *mesh)
	{
		if (root!=NULL)
			insert(mesh,root,0);
		else
		{
			root = new kdNode;
			root->mesh = mesh;
			root->left = NULL;
			root->right = NULL;
		}
	};
	std::vector<Mesh*> search(vec3 center, float radius)
	{
		searchResults.clear();
		minV=center-radius;
		maxV=center+radius;
		search(root,center,radius,0);
		return searchResults;
	};
	~kdTree()
	{
		destroy(root);
	};
private:
	void destroy(kdNode *leaf)
	{
		if (leaf!=NULL)
		{
			destroy(leaf->left);
			destroy(leaf->right);
			delete leaf;
		}
	};
	void insert(Mesh *mesh, kdNode *leaf,int level)
	{
		if (mesh->center[level%3] <= leaf->mesh->center[level%3])
			if (leaf->left!=NULL)
			{
				insert(mesh,leaf->left,level++);
			}
			else
			{
				leaf->left = new kdNode;
				leaf->left->mesh = mesh;
				leaf->left->left = NULL;
				leaf->left->right = NULL;
			}
		else
			if (leaf->right!=NULL)
			{
				insert(mesh,leaf->right,level++);
			}
			else
			{
				leaf->right = new kdNode;
				leaf->right->mesh = mesh;
				leaf->right->left = NULL;
				leaf->right->right = NULL;
			}
	}
	void search(kdNode *leaf,vec3 &center, float radius, int level)
	{
		if (leaf==NULL)
			return;
		if (distance(leaf,center) <= radius)
			searchResults.push_back(leaf->mesh);

		int dim=level%3;

		if (leaf->mesh->center[dim] < minV[dim])
			search(leaf->right,center,radius,dim++);
		else
			search(leaf->left,center,radius,dim++);

		if (leaf->mesh->center[dim] >= maxV[dim])
		{
			if (leaf->mesh->center[dim] < minV[dim])
			search(leaf->left,center,radius,dim++);
		}
		else
		{
			if (leaf->mesh->center[dim] >= minV[dim])
			search(leaf->right,center,radius,dim++);
		}

	}
	inline float distance(kdNode *m1,vec3 center)
	{
		return glm::distance(m1->mesh->center,center);
	}
	kdNode *root;
	std::vector<Mesh*> searchResults;
	vec3 minV,maxV;
};

#endif