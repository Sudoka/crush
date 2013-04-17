//=================================================================================================
// GameResources.cpp Static container for game resources including meshes, objects, cameras, etc.
//=================================================================================================


// Global includes
#include <algorithm>

// Project includes
#include <client/GameResources.h>
#include <client/Gbls.h>
#include <client/graphics/Skybox.h>
#include <client/graphics/entities/R_Ship.h>

//static member initializations
//static enum cameras {DEBUG_CAM, PLAYER_CAM};  //better to use boolean with only two cameras, can extend later
bool GameResources::debugCamOn = true;
Camera GameResources::debugCam;
Camera* GameResources::curCam = NULL;
//std::vector<R_Ship*> GameResources::r_ShipList;
std::vector<Entity*> GameResources::entityList;
//std::vector<std::vector<Renderable*>*> GameResources::renderList;
struct GameResources::KeyboardState GameResources::m_ks;

HRESULT GameResources::initState() {
	HRESULT hres;
	
	curCam = &debugCam;

	debugCam.updateProjection();

	debugCam.updateView();

	// Tell the device to automatically normalize surface normals to keep them normal after scaling
	Gbls::pd3dDevice->SetRenderState(D3DRS_NORMALIZENORMALS, TRUE);

	// Initialize the skybox
	hres = Skybox::initSkybox();
	if(FAILED (hres))
		return hres;

	// Create lights for scene and set light properties
	hres = GameResources::initLights();
	if(FAILED (hres))
		return hres;

	// Clear keyboard state (at the moment only used for debug camera 4/13/2013)
	memset(&GameResources::m_ks, 0, sizeof(GameResources::KeyboardState));

	// create meshes
	hres = GameResources::initMeshes();
	if(FAILED (hres))
		return hres;

	/*set up temp entities for test rendering TODO remove this and replace with normal object creation from network*/
	
	D3DXVECTOR3 pos(0.0f, 1.0f, -1.0f);
	D3DXVECTOR3 dir(0.0f, 0.0f, 1.0f);
	int pNum = 1;
	D3DXCOLOR color(0.8f, 0.3f, 0.3f, 0.5f);
	bool tBeamOn = false;
	R_Ship* tmp = new R_Ship(pos, dir, pNum, tBeamOn, &Gbls::shipMesh2, color);
	Mesh::setScaleRotate(tmp->m_matInitScaleRot, 0.005f, -90.0f, 180.0f, 0.0f);
	entityList.push_back(tmp);

	//pos.x*=-1; pos.y*=-1; pos.z*=-1;
	//dir.x*=-1; dir.y*=-1; dir.z*=-1;
	pos.y = 0.3f; pos.z = -0.6f;
	pNum = 2;
	color.r = 0.3f; color.g = 0.3f; color.b = 0.8f;
	tmp = new R_Ship(pos, dir, pNum, tBeamOn, &Gbls::shipMesh1, color);
	entityList.push_back(tmp);

	//a bit ugly, probably easier to just loop through all the entity lists (left here in case we want to switch back)
	//renderList.push_back((std::vector<Renderable*>*)(&r_ShipList));

	/*end TODO remove*/

	return S_OK;
}

HRESULT GameResources::initMeshes()
{
	HRESULT hres;

	//do for all needed meshes
	if(FAILED(hres = Gbls::shipMesh1.Create(Gbls::shipMeshFilepath_1)))
		return hres;
	if(FAILED(hres = Gbls::shipMesh2.Create(Gbls::shipMeshFilepath_2)))
		return hres;

	return S_OK;
}

HRESULT GameResources::initLights() {
	// Tell the device not to do any dynamic lighting
	//Gbls::pd3dDevice->SetRenderState(D3DRS_LIGHTING, FALSE);

	// turn on specular highlights
	Gbls::pd3dDevice->SetRenderState(D3DRS_SPECULARENABLE, TRUE);

	// ambient light
	Gbls::pd3dDevice->SetRenderState(D3DRS_AMBIENT, Gbls::lightAmbient);

	// directional light
    D3DLIGHT9 light;    // create the light struct

    ZeroMemory(&light, sizeof(light));         // clear out the light struct for use
    light.Type = D3DLIGHT_DIRECTIONAL;         // make the light type 'directional light'
    light.Diffuse = Gbls::lightDiffuseColor;   // set the light's diffuse color
	light.Specular = Gbls::lightSpecularColor; // set the light's specular color
    light.Direction = Gbls::lightDirection;    // set the light's direction
    //D3DXVec3Normalize((D3DXVECTOR3*) &light.Direction, &D3DXVECTOR3(-1.0f, -0.3f, -1.0f));
	light.Range = 1000;
	light.Falloff = 0;
	light.Attenuation0 = 1;
	light.Attenuation1 = 0;
	light.Attenuation2 = 0;

    Gbls::pd3dDevice->SetLight(0, &light);    // send the light struct properties to light #0
    Gbls::pd3dDevice->LightEnable(0, TRUE);    // turn on light #0
	
	return S_OK;
}

void GameResources::drawAll()
{
	Skybox::drawSkybox();

	// Loop through all lists. Set up shaders, etc, as needed for each.
	for (DWORD i = 0; i < entityList.size(); i++) {
		entityList.at(i)->draw();
	}
}

// called each frame after processing keyboard state from that frame
// clears out up keys
void GameResources::updateKeyboardState() {
	m_ks.wUp = false;
	m_ks.aUp = false;
	m_ks.sUp = false;
	m_ks.dUp = false;
	m_ks.upUp = false;
	m_ks.leftUp = false;
	m_ks.downUp = false;
	m_ks.rightUp = false;
}

void GameResources::updateDebugCamera() {
	
	INT8	updateFwd = (INT8)(m_ks.wDown || m_ks.wUp) - (INT8)(m_ks.sDown || m_ks.wUp);
	INT8	updateStrafe = (INT8)(m_ks.dDown || m_ks.dUp) - (INT8)(m_ks.aDown || m_ks.aUp);
	INT8	updateYaw = (INT8)(m_ks.rightDown || m_ks.rightUp) - (INT8)(m_ks.leftDown || m_ks.leftUp);
	INT8	updatePitch =  (INT8)(m_ks.downDown || m_ks.downUp) - (INT8)(m_ks.upDown || m_ks.upUp);

	if(!debugCamOn || !updateFwd && !updateStrafe && !updateYaw && !updatePitch)
		return;

	if(!debugCamOn) {
		return;
	}
	
	D3DXVECTOR3 fwdVec, rightVec;

	if(updateYaw || updatePitch) { // update direction
		debugCam.m_yaw += updateYaw*D3DXToRadian(Gbls::debugCamTurnSpeed);
		debugCam.m_pitch += updatePitch*D3DXToRadian(Gbls::debugCamTurnSpeed);
		debugCam.m_pitch = max(Gbls::debugCamMinPitch, min(Gbls::debugCamMaxPitch, debugCam.m_pitch)); // clip m_pitch
	}

	if(updateFwd || updateStrafe) { // update position
		fwdVec = debugCam.m_vAt-debugCam.m_vEye; // get fwd vector
		D3DXVec3Normalize(&fwdVec, &fwdVec);
		D3DXVec3Cross(&rightVec, &(debugCam.m_vUp), &fwdVec); // get right vector
		D3DXVec3Normalize(&rightVec, &rightVec);

		if(updateFwd) {
			debugCam.m_vEye = debugCam.m_vEye + fwdVec*(Gbls::debugCamMoveSpeed*updateFwd);
			//D3DXVec3Normalize(&debugCam.m_vEye, &debugCam.m_vEye);
		}

		if(updateStrafe) {
			debugCam.m_vEye = debugCam.m_vEye + rightVec*(Gbls::debugCamMoveSpeed*updateStrafe);
			//D3DXVec3Normalize(&debugCam.m_vEye, &debugCam.m_vEye);
		}
	}

	//update lookAt vector & up vector (must be done for both position and direction udpates)
	D3DXMATRIX yawMatrix;
	D3DXMATRIX pitchMatrix;

	//define direction vectors
	debugCam.m_vUp = D3DXVECTOR3(0.0f, 1.0f, 0.0f);
    rightVec  = D3DXVECTOR3(1.0f, 0.0f, 0.0f);
    fwdVec  = D3DXVECTOR3(0.0f, 0.0f, 1.0f);

	D3DXMatrixRotationAxis(&pitchMatrix, &rightVec, debugCam.m_pitch);
	D3DXMatrixRotationAxis(&yawMatrix, &debugCam.m_vUp, debugCam.m_yaw);
	
	//compute direction (fwd) vector
    D3DXVec3TransformCoord(&fwdVec, &fwdVec, &pitchMatrix);
    D3DXVec3TransformCoord(&fwdVec, &fwdVec, &yawMatrix);
    D3DXVec3Normalize(&fwdVec, &fwdVec);

	//compute right vector (to update up vector)
    D3DXVec3Cross(&rightVec, &debugCam.m_vUp, &fwdVec);
    D3DXVec3Normalize(&rightVec, &rightVec);

	//update up vector
    D3DXVec3Cross(&debugCam.m_vUp, &fwdVec, &rightVec);

	//update lookAt vector
	debugCam.m_vAt = debugCam.m_vEye + fwdVec;

	//update directx camera view
	GameResources::debugCam.updateView();

}

void GameResources::updateGameState(GameState & newGameState) {
	updateDebugCamera();
	updateKeyboardState();

	//TODO THIS IS N^2. FIX THIS.
	for (DWORD i = 0; i < newGameState.size(); i++) {
		for (DWORD j = 0; j < entityList.size(); j++) {
			if(newGameState[i]->getID() == entityList.at(j)->getID()) {
				entityList.at(j)->update(newGameState[i]);
			}
		}
	}
	//for (DWORD i = 0; i < r_ShipList.size(); i++) {
	//	float move = i%2 ? 0.001f : -0.001f;
	//	r_ShipList.at(i)->m_pos.y += move;
	//}
}

void GameResources::releaseResources() {
	Gbls::shipMesh2.Destroy();
	Gbls::shipMesh1.Destroy();
	Skybox::releaseSkybox();
}