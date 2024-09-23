/*
Author: Nathan Dunn
Module: G3D

Tile object intended to represent the ground.
This version of tile has dynamic vertices.
It is the nature of tiles that the vertices are automatically generated. However,
Thumper's tiles can have their z values set to produce more real terrain. To do
this, call SetVertexZ before Draw is called. Same with SetYaw.

Known bugs and limitations:
- The current map functionality does NOT allow for changing the number of vertices, only editing existing vertices
- Calls to BuildGeometry that increase buffer sizes will cause a memory overflow when Map is next called

*/

#pragma once
#include "G3D_RenderEngine.h"
#include "G3D_Utilities.h"
#include "G3D_Shader_Texture.h"

namespace G3D
{
	class Obj_Tile;
	enum TileGeometry;
}

/*
Utilized in the constructors to identify what vertex arrangement the
tile will use. Add an entry here (and in the Asset File Definitions
spreadsheet) for each new vertex arrangement. Also add a new line
in TileVertexArrangement() and TileIndexArrangement().
*/
enum G3D::TileGeometry : char
{
	Null,
	Single,
	Double,
	TransitionA,
	Star,
	Triple
};

class G3D::Obj_Tile
{
public:
	// public member functions
	Obj_Tile() = delete;
	Obj_Tile(G3D::RenderEngine& RE, G3D::Shader_Texture& Shader, G3D::TileGeometry Layout, float Yaw, float XPosition, float YPosition);
	void Create();
	void Draw();
	void SetVertexZ(int Index, float Z);
	void SetYaw(float NewYaw);
	void SetDrawFlag(bool State);

private:
	// private member functions
	void CalculateTransform(); // in the future send orientation data directly to the GPU for the VS to calculate
	void Map();
	void BuildGeometry();

public:
	// public member variables
	HRESULT HR;

private:
	// private member variables
	G3D::RenderEngine& RE;
	G3D::TileGeometry Layout;
	G3D::Shader_Texture& Shader;
	std::vector<G3D::Vertex_PT> Vertices;
	std::vector<unsigned short> Indices;
	float XPosition;
	float YPosition;
	float Yaw;
	bool FlagDraw;
	bool CreationComplete;
	bool OptionalsNeedMapped;
	Microsoft::WRL::ComPtr<ID3D11Buffer> pVertexBuffer;
	Microsoft::WRL::ComPtr<ID3D11Buffer> pIndexBuffer;
	Microsoft::WRL::ComPtr<ID3D11Buffer> pConstantBuffer;
	DirectX::XMMATRIX Transform;

};