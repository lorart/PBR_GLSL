#include "Camera.h"
#include "Window.h"
#include <algorithm>

using namespace NCL;

/*
Polls the camera for keyboard / mouse movement.
Should be done once per frame! Pass it the msec since
last frame (default value is for simplicities sake...)
*/
void Camera::UpdateCamera(float dt) {
	//Update the mouse by how much
	pitch	-= (Window::GetMouse()->GetRelativePosition().y);
	yaw		-= (Window::GetMouse()->GetRelativePosition().x);

	//Bounds check the pitch, to be between straight up and straight down ;)
	pitch = std::min(pitch, 90.0f);
	pitch = std::max(pitch, -90.0f);

	if (yaw <0) {
		yaw += 360.0f;
	}
	if (yaw > 360.0f) {
		yaw -= 360.0f;
	}

	float frameSpeed = 200 * dt;

	if (Window::GetKeyboard()->KeyDown(KeyboardKeys::W)) {
		position += Matrix4::Rotation(yaw, Vector3(0, 1, 0)) * Vector3(0, 0, -1) * frameSpeed;
	}
	if (Window::GetKeyboard()->KeyDown(KeyboardKeys::S)) {
		position -= Matrix4::Rotation(yaw, Vector3(0, 1, 0)) * Vector3(0, 0, -1) * frameSpeed;
	}

	if (Window::GetKeyboard()->KeyDown(KeyboardKeys::A)) {
		position += Matrix4::Rotation(yaw, Vector3(0, 1, 0)) * Vector3(-1, 0, 0) * frameSpeed;
	}
	if (Window::GetKeyboard()->KeyDown(KeyboardKeys::D)) {
		position -= Matrix4::Rotation(yaw, Vector3(0, 1, 0)) * Vector3(-1, 0, 0) * frameSpeed;
	}

	if (Window::GetKeyboard()->KeyDown(KeyboardKeys::SHIFT)) {
		position.y += frameSpeed;
	}
	if (Window::GetKeyboard()->KeyDown(KeyboardKeys::SPACE)) {
		position.y -= frameSpeed;
	}
}

/*
Generates a view matrix for the camera's viewpoint. This matrix can be sent
straight to the shader...it's already an 'inverse camera' matrix.
*/
Matrix4 Camera::BuildViewMatrix() const {
	//Why do a complicated matrix inversion, when we can just generate the matrix
	//using the negative values ;). The matrix multiplication order is important!
	return	Matrix4::Rotation(-pitch, Vector3(1, 0, 0)) *
		Matrix4::Rotation(-yaw, Vector3(0, 1, 0)) *
		Matrix4::Translation(-position);
};

Matrix4 Camera::BuildProjectionMatrix(float currentAspect) const {
	if (camType == CameraType::Orthographic) {
		return Matrix4::Orthographic(nearPlane, farPlane, right, left, top, bottom);
	}
	//else if (camType == CameraType::Perspective) {
		return Matrix4::Perspective(nearPlane, farPlane, currentAspect, fov);
	//}
}

Camera Camera::BuildPerspectiveCamera(const Vector3& pos, float pitch, float yaw, float fov, float nearPlane, float farPlane) {
	Camera c;
	c.camType	= CameraType::Perspective;
	c.position	= pos;
	c.pitch		= pitch;
	c.yaw		= yaw;
	c.nearPlane = nearPlane;
	c.farPlane  = farPlane;

	c.fov		= fov;

	return c;
}
Camera Camera::BuildOrthoCamera(const Vector3& pos, float pitch, float yaw, float left, float right, float top, float bottom, float nearPlane, float farPlane) {
	Camera c;
	c.camType	= CameraType::Orthographic;
	c.position	= pos;
	c.pitch		= pitch;
	c.yaw		= yaw;
	c.nearPlane = nearPlane;
	c.farPlane	= farPlane;

	c.left		= left;
	c.right		= right;
	c.top		= top;
	c.bottom	= bottom;

	return c;
}

Camera Camera::BuildPinholeCamera(const Vector3& pos, float pitch, float yaw, float fov, float near, float far, int lens)
{
	Camera c;
	c.camType = CameraType::Perspective;
	c.position = pos;
	c.pitch = pitch;
	c.yaw = yaw;
	c.nearPlane = near;
	c.farPlane = far;

	c.fov = fov;

	c.lens = lens;

	return c;

	//todo: more
}

Camera Camera::BuildVOFCamera(const Vector3& pos, float pitch, float yaw, float fov, float near, float far, int nearDistance, int farDistance)
{
	Camera c;
	c.camType = CameraType::Perspective;
	c.position = pos;
	c.pitch = pitch;
	c.yaw = yaw;
	c.nearPlane = near;
	c.farPlane = far;

	c.fov = fov;
	c.nearDistance = nearDistance;
	c.farDistance = farDistance;
	

	return c;

}
float Camera::caculateFieldofView()
{

	
	return(atan(43.3 / (2 * this->lens))*2*180/3.14159);
}

float  Camera::caculateAlphaY() {
	//appropriate by the given alphaX Y 
	if (this->lens < 35) {
		return 0.075;

	}
	//35-40
	else if (this->lens >= 35 && this->lens <= 40) {
		return 0.075 + (this->lens - 35) / 5 * (0.059 - 0.075);

	}

	//40-50
	else if (this->lens > 40 && this->lens <= 50) {
		return 0.059 + (this->lens - 40) / 10 * (0.03 - 0.059);
	}
	else {
		return 0.03;
	}
}

float  Camera::caculateAlphaX() {
	//appropriate by the given alphaX Y 

	//35-40
	if (this->lens < 35) {
		return 0.02;

	}
	else if (this->lens >= 35 && this->lens <= 40) {
		return 0.02 + (this->lens - 35) / 5 * (0.016 - 0.02);

	}
	//40-50
	else if (this->lens > 40 && this->lens <= 50) {
		
		return 0.016 + (this->lens - 40) / 10 * (0.006 - 0.016);
	}
	else {
		return 0.006;
	}
}