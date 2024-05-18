#include "scene_manager.h"

#include <Windows.h>
#include <gl/GL.h>
#include <gl/GLU.h>
#include "GL/freeglut.h"
#include "glut_callback_handlers.h"
#include "matrix3.h"
#include <queue>
#include <string>
#include "multi_game.h"

#define _USE_MATH_DEFINES
#include <math.h>

#include <iostream>

inline float randf()
{
	return (((float)rand() / (float)RAND_MAX) * 2.0f) - 1.0f;
}

SceneManager::SceneManager()
{
}

void SceneManager::initialise(int argc, char* argv[], unsigned int x, unsigned int y)
{
	glut_callback_handlers::init(this);

	last_frame_time = std::chrono::high_resolution_clock::now();

	viewport_width = x;
	viewport_height = y;

	srand((unsigned int)last_frame_time.time_since_epoch().count());

	// core scene initialisation
	world_root = new Object();
	world_root->name = "root";
	active_camera = new CameraObject(90.0f, 0.01f, 64.0f, (float)x / (float)y, Vector3{ 0,-1,1 }, Vector3{ -23.0f,0,0 });
	world_root->addChild(active_camera);

	overlay_root = new Object();
	overlay_root->name = "overlay";

	// GLUT and GL initialisation
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_RGB | GLUT_DEPTH);
	glutInitWindowSize(x, y);
	glutInitWindowPosition(50, 50);
	glutCreateWindow("GP");
	// configure callbacks
	glutDisplayFunc(glut_callback_handlers::display);
	glutMotionFunc(glut_callback_handlers::mouseMove);
	glutPassiveMotionFunc(glut_callback_handlers::mouseMovePassive);
	glutMouseFunc(glut_callback_handlers::mouseClick);
	glutKeyboardFunc(glut_callback_handlers::keyDown);
	glutKeyboardUpFunc(glut_callback_handlers::keyUp);
	glutIgnoreKeyRepeat(1);
	glutReshapeFunc(glut_callback_handlers::resizeWindow);
	glutTimerFunc(1000 / 60, glut_callback_handlers::frameRefresh, 1000 / 60);
	glutCreateMenu(glut_callback_handlers::menuFunc);
	// configure menu
	glutSetMenu(0);
	glutAddMenuEntry("switch realities", 1);
	glutAddMenuEntry("toggle postprocessing", 2);
	glutAttachMenu(GLUT_RIGHT_BUTTON);
	// enable depth buffer/testing
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LESS);
	// cull backfaces
	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);
	// counter-clockwise winding
	glFrontFace(GL_CCW);
	// enable smooth lighting
	glEnable(GL_LIGHTING);
	glShadeModel(GL_SMOOTH);
	// enable textures
	glEnable(GL_TEXTURE_2D);
	glDisable(GL_COLOR_MATERIAL);
	// enable normalisation, since we're scaling objects
	glEnable(GL_NORMALIZE);

	// load LUT
	Vector3* lut_input_buffer;
	lut_buffer = new Vector3[512 * 512];
	Texture::loadBMPRaw("lut.bmp", lut_input_buffer);
	for (int x = 0; x < 64; x++)
	{
		for (int y = 0; y < 64; y++)
		{
			for (int z = 0; z < 64; z++)
			{
				lut_buffer[x + (y * 64) + (z * 64 * 64)] = lut_input_buffer[x + (y * 64 * 8) + ((z % 8) * 64) + ((z >> 3) * 64 * 64 * 8)];
			}
		}
	}
	delete lut_input_buffer;
}

void SceneManager::startMainloop(GameManager* game)
{
	setGameManager(game);

	// aaaaaand, go!
	glutMainLoop();
}

void SceneManager::display()
{	
	// clear buffers
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	if (active_camera)
	{
		renderFromCamera(active_camera);
		performPostProcessing(active_camera);
	}

	glFlush();
}

void SceneManager::mouseMove(int x, int y)
{
	int diff_x = x - last_mouse_x;
	int diff_y = y - last_mouse_y;
	last_mouse_x = x;
	last_mouse_y = y;

	game_manager->mouseMove(diff_x, diff_y, true);
}

void SceneManager::mouseMovePassive(int x, int y)
{
	int diff_x = x - last_mouse_x;
	int diff_y = y - last_mouse_y;
	last_mouse_x = x;
	last_mouse_y = y;

	game_manager->mouseMove(diff_x, diff_y, false);
}

void SceneManager::mouseClick(int button, int state, int x, int y)
{
	game_manager->mousePressed(button, state, x, y);
}

void SceneManager::keyDown(uint8_t key, int x, int y)
{
	game_manager->keyPressed(key, true);
}

void SceneManager::keyUp(uint8_t key, int x, int y)
{
	game_manager->keyPressed(key, false);
}

void SceneManager::frameRefresh(int value)
{
	// get delta time
	auto time_now = std::chrono::high_resolution_clock::now();
	float delta_time = std::chrono::duration_cast<std::chrono::nanoseconds>(time_now - last_frame_time).count() / 1000000000.0f;
	last_frame_time = time_now;
	current_fps = (0.2f * current_fps) + (1.0f / (0.8f * delta_time));

	game_manager->update(delta_time);

	// perform physics update for all objects (and tick particle lifetimes)
	std::queue<Object*> physics_tick_queue;
	physics_tick_queue.push(world_root);
	while (!physics_tick_queue.empty())
	{
		Object* obj = physics_tick_queue.front();
		physics_tick_queue.pop();

		obj->performPhysicsUpdate(delta_time);
		
		if (obj->getType() == ObjectType::PARTICLE)
		{
			ParticleObject* pobj = (ParticleObject*)obj;
			pobj->time_alive += delta_time;
			if (pobj->time_alive >= pobj->lifetime)
				pobj->destroy();
		}

		for (size_t i = 0; i < obj->children.getLength(); i++) physics_tick_queue.push(obj->children[i]);
	}
	// and all overlay objects
	for (size_t i = 0; i < overlay_root->children.getLength(); i++)
	{
		overlay_root->children[i]->performPhysicsUpdate(delta_time);
	}

	// check for dead objects and delete them
	std::queue<Object*> destruction_queue;
	destruction_queue.push(world_root);
	while (!destruction_queue.empty())
	{
		Object* obj = destruction_queue.front();
		destruction_queue.pop();

		if (obj->is_waiting_for_death)
			delete obj;
		else
			for (size_t i = 0; i < obj->children.getLength(); i++) destruction_queue.push(obj->children[i]);
	}

	// check for errors
	GLenum err;
	while ((err = glGetError()) != GL_NO_ERROR)
	{
		std::cout << "ERROR DETECTED:" << std::endl << "    ";
		switch (err)
		{
		case GL_INVALID_ENUM: std::cout << "invalid enum"; break;
		case GL_INVALID_VALUE: std::cout << "invalid value"; break;
		case GL_INVALID_OPERATION: std::cout << "invalid operation"; break;
		case GL_STACK_OVERFLOW: std::cout << "stack overflow"; break;
		case GL_STACK_UNDERFLOW: std::cout << "stack underflow"; break;
		case GL_OUT_OF_MEMORY: std::cout << "out of memory"; break;
		}
		std::cout << std::endl;
	}

	// trigger drawing routine
	glutPostRedisplay();
	glutTimerFunc(value, glut_callback_handlers::frameRefresh, value);
}

void SceneManager::resizeWindow(int x, int y)
{
	glViewport(0, 0, x, y);
	viewport_width = x;
	viewport_height = y;
	if (post_processing_buffer)
	{
		delete[] post_processing_buffer;
		post_processing_buffer = NULL;
	}
	active_camera->aspect_ratio = (float)x / (float)y;
}

void SceneManager::addObject(Object* obj)
{
	if (!obj) return;

	world_root->addChild(obj);
}

void SceneManager::renderFromCamera(CameraObject* camera)
{
	// if the camera supplied is null, return
	if (!camera) return;

	// set the projection matrix to be a simple perspective matrix
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(camera->fov_degrees, camera->aspect_ratio, camera->near_clip, camera->far_clip);

	// compute a matrix for the camera's transform (i.e. world-to-view)
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	Object* camera_matrix_stack = camera;
	while (camera_matrix_stack != NULL)
	{
		glScalef(1.0f / camera_matrix_stack->local_scale.x, 1.0f / camera_matrix_stack->local_scale.y, 1.0f / camera_matrix_stack->local_scale.z);
		glRotatef(camera_matrix_stack->local_rotation.y, 0.0f, 1.0f, 0.0f);
		glRotatef(camera_matrix_stack->local_rotation.x, 1.0f, 0.0f, 0.0f);
		glRotatef(camera_matrix_stack->local_rotation.z, 0.0f, 0.0f, 1.0f);
		glTranslatef(-camera_matrix_stack->local_position.x, -camera_matrix_stack->local_position.y, -camera_matrix_stack->local_position.z);
		
		camera_matrix_stack = camera_matrix_stack->parent;
	}
	
	// configure lights
	updateLights();

	// render the entire object heirarchy
	if (world_root)
		renderHierarchy(world_root);

	// render the background
	drawEnvironmentCubemap(camera);

	// render the gizmo
	drawOverlay(camera);
}

void SceneManager::renderHierarchy(Object* root)
{
	if (!root) return;

	// push the matrix down
	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();

	// apply transformation matrix of root
	glTranslatef(root->local_position.x, root->local_position.y, root->local_position.z);
	glRotatef(-root->local_rotation.z, 0.0f, 0.0f, 1.0f);
	glRotatef(-root->local_rotation.x, 1.0f, 0.0f, 0.0f);
	glRotatef(-root->local_rotation.y, 0.0f, 1.0f, 0.0f);
	glScalef(root->local_scale.x, root->local_scale.y, root->local_scale.z);

	// draw root
	objects_drawn_last_frame++;
	if (root->getType() == ObjectType::MESH)
		drawObject((MeshObject*)root);
	if (root->getType() == ObjectType::PARTICLE)
		drawParticle((ParticleObject*)root);

	// iterate over children, calling renderHierarchy on each
	for (size_t i = 0; i < root->children.getLength(); i++)
	{
		if (!root->children[i]->is_waiting_for_death)
			renderHierarchy(root->children[i]);
	}
	
	// pop matrix
	glPopMatrix();
}

void SceneManager::drawEnvironmentCubemap(CameraObject* camera)
{
	if (!camera) return;

	// compute a matrix for the camera's transform (i.e. world-to-view), but only its rotation
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	Object* camera_matrix_stack = camera;
	while (camera_matrix_stack != NULL)
	{
		glRotatef(camera_matrix_stack->local_rotation.y, 0.0f, 1.0f, 0.0f);
		glRotatef(camera_matrix_stack->local_rotation.x, 1.0f, 0.0f, 0.0f);
		glRotatef(camera_matrix_stack->local_rotation.z, 0.0f, 0.0f, 1.0f);

		camera_matrix_stack = camera_matrix_stack->parent;
	}

	glBindTexture(GL_TEXTURE_2D, 0);
	if (skybox)
	{
		if (skybox->getID() != (unsigned int)-1)
			glBindTexture(GL_TEXTURE_2D, skybox->getID());
		else
			std::cout << "invalid environment texture!" << std::endl;
	}
	else std::cout << "no skybox texture assigned" << std::endl;

	glDisable(GL_LIGHTING);
	glDisable(GL_FOG);
	glColor3f(1.5f, 1.5f, 1.5f);
	
	// inscribed dimension of cube
	float inscribed = camera->far_clip / sqrt(3.0f);
	// +X face
	float max = 1.0f;
	glBegin(GL_QUADS);
	{
		glTexCoord2f(0.0f/6.0f, max);
		glVertex3f(inscribed, inscribed, inscribed);
		glTexCoord2f(0.0f/6.0f, 0.0f);
		glVertex3f(inscribed, inscribed, -inscribed);
		glTexCoord2f(1.0f/6.0f, 0.0f);
		glVertex3f(inscribed, -inscribed, -inscribed);
		glTexCoord2f(1.0f / 6.0f, max);
		glVertex3f(inscribed, -inscribed, inscribed);

	}
	glEnd();
	// +Y face (sampling +Z of cubemap)
	//glColor3f(0.0f, 1.0f, 0.0f);
	glBegin(GL_QUADS);
	{
		glTexCoord2f(4.0f / 6.0f, max);
		glVertex3f(-inscribed, inscribed, inscribed);
		glTexCoord2f(4.0f / 6.0f, 0.0f);
		glVertex3f(-inscribed, inscribed, -inscribed);
		glTexCoord2f(5.0f / 6.0f, 0.0f);
		glVertex3f(inscribed, inscribed, -inscribed);
		glTexCoord2f(5.0f / 6.0f, max);
		glVertex3f(inscribed, inscribed, inscribed);

	}
	glEnd();
	// -X face
	glBegin(GL_QUADS);
	{
		glTexCoord2f(1.0f / 6.0f, max);
		glVertex3f(-inscribed, -inscribed, inscribed);
		glTexCoord2f(1.0f / 6.0f, 0.0f);
		glVertex3f(-inscribed, -inscribed, -inscribed);
		glTexCoord2f(2.0f / 6.0f, 0.0f);
		glVertex3f(-inscribed, inscribed, -inscribed);
		glTexCoord2f(2.0f / 6.0f, max);
		glVertex3f(-inscribed, inscribed, inscribed);
	}
	glEnd();
	// -Y face (sampling -Z of cubemap)
	glBegin(GL_QUADS);
	{
		glTexCoord2f(5.0f / 6.0f, max);
		glVertex3f(inscribed, -inscribed, inscribed);
		glTexCoord2f(5.0f / 6.0f, 0.0f);
		glVertex3f(inscribed, -inscribed, -inscribed);
		glTexCoord2f(6.0f / 6.0f, 0.0f);
		glVertex3f(-inscribed, -inscribed, -inscribed);
		glTexCoord2f(6.0f / 6.0f, max);
		glVertex3f(-inscribed, -inscribed, inscribed);
	}
	glEnd();
	// +Z face (sampling +Y of cubemap) FIXME: this might need rotating!
	glBegin(GL_QUADS);
	{
		glTexCoord2f(3.0f / 6.0f, 0.0f);
		glVertex3f(inscribed, inscribed, inscribed);
		glTexCoord2f(3.0f / 6.0f, max);
		glVertex3f(inscribed, -inscribed, inscribed);
		glTexCoord2f(2.0f / 6.0f, max);
		glVertex3f(-inscribed, -inscribed, inscribed);
		glTexCoord2f(2.0f / 6.0f, 0.0f);
		glVertex3f(-inscribed, inscribed, inscribed);
	}
	glEnd();
	// -Z face (sampling -Y of cubemap) FIXME: this might need rotating!
	glBegin(GL_QUADS);
	{
		glTexCoord2f(3.0f / 6.0f, max);
		glVertex3f(-inscribed, inscribed, -inscribed);
		glTexCoord2f(3.0f / 6.0f, 0.0f);
		glVertex3f(-inscribed, -inscribed, -inscribed);
		glTexCoord2f(4.0f / 6.0f, 0.0f);
		glVertex3f(inscribed, -inscribed, -inscribed);
		glTexCoord2f(4.0f / 6.0f, max);
		glVertex3f(inscribed, inscribed, -inscribed);
	}
	glEnd();

	glEnable(GL_LIGHTING);
	glEnable(GL_FOG);
}

void SceneManager::drawOverlay(CameraObject* camera)
{
	if (!camera) return;

	// reset the world-to-view camera stack, moving the axes to the top left of the screen
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	// orthographic projection
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glOrtho(-camera->aspect_ratio, camera->aspect_ratio, -1, 1, -1, 1);

	// disable lighting
	glDisable(GL_LIGHTING);
	glDisable(GL_FOG);
	glBindTexture(GL_TEXTURE_2D, 0);

	// draw three lines on the three axes (coloured accordingly)
	glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	
	glMatrixMode(GL_MODELVIEW);
	for (size_t i = 0; i < overlay_root->children.getLength(); i++)
	{
		Object* child_object = overlay_root->children[i];
		objects_drawn_last_frame++;
		if (child_object->getType() == ObjectType::MESH)
		{
			MeshObject* child_mesh_object = (MeshObject*)child_object;
			if (child_mesh_object->geometry == NULL) continue;
			glPushMatrix();
			glTranslatef(child_object->local_position.x * camera->aspect_ratio, child_object->local_position.y, 0);
			glRotatef(-child_object->local_rotation.z, 0.0f, 0.0f, 1.0f);
			glRotatef(-child_object->local_rotation.x, 1.0f, 0.0f, 0.0f);
			glRotatef(-child_object->local_rotation.y, 0.0f, 1.0f, 0.0f);
			glScalef(child_object->local_scale.x, child_object->local_scale.y, child_object->local_scale.z);
			glColor3f(0.2f, 0.9f, 0.1f);
			glBegin(GL_LINES);
			{
				for (size_t i = 0; i < child_mesh_object->geometry->trisCount() - 1; i++)
				{
					Vector3 vert_a = child_mesh_object->geometry->vertices[child_mesh_object->geometry->triangles[i]];
					Vector3 vert_b = child_mesh_object->geometry->vertices[child_mesh_object->geometry->triangles[i + 1]];
					glVertex3f(vert_a.x, vert_a.y, vert_a.z);
					glVertex3f(vert_b.x, vert_b.y, vert_b.z);
				}
			}
			glEnd();
			glPopMatrix();
		}
		else if (child_object->getType() == ObjectType::TEXT)
		{
			TextObject* child_text_object = (TextObject*)child_object;
			Vector3 col = child_text_object->colour;
			glColor4f(col.x, col.y, col.z, 1.0f);
			glRasterPos2f(child_text_object->raster_position.x, child_text_object->raster_position.y);
			glutBitmapString(child_text_object->font, (const unsigned char*)child_text_object->text.c_str());
		}
	}

	glColor4f(1.0f, 1.0f, 1.0f, 1.0f);
	glRasterPos2f(-0.6f, 0.8f);
	glutBitmapString(GLUT_BITMAP_HELVETICA_12, (const unsigned char*)("objects: " + std::to_string(objects_drawn_last_frame)).c_str());
	glRasterPos2f(-0.6f, 0.75f);
	glutBitmapString(GLUT_BITMAP_HELVETICA_12, (const unsigned char*)("triangles: " + std::to_string(triangles_drawn_last_frame)).c_str());
	glRasterPos2f(-0.6f, 0.7f);
	glutBitmapString(GLUT_BITMAP_HELVETICA_12, (const unsigned char*)("fps: " + std::to_string(current_fps)).c_str());

	objects_drawn_last_frame = 0;
	triangles_drawn_last_frame = 0;

	glTranslatef(-camera->aspect_ratio + 0.2f, 0.8f, -0.8f);
	Object* camera_matrix_stack = camera;
	while (camera_matrix_stack != NULL)
	{
		glRotatef(camera_matrix_stack->local_rotation.y, 0.0f, 1.0f, 0.0f);
		glRotatef(camera_matrix_stack->local_rotation.x, 1.0f, 0.0f, 0.0f);
		glRotatef(camera_matrix_stack->local_rotation.z, 0.0f, 0.0f, 1.0f);

		camera_matrix_stack = camera_matrix_stack->parent;
	}

	glBegin(GL_LINES);
	{
		glColor3f(1, 0, 0);
		glVertex3f(0, 0, 0);
		glVertex3f(0.18f, 0, 0);

		glColor3f(0, 1, 0);
		glVertex3f(0, 0, 0);
		glVertex3f(0, 0.18f, 0);

		glColor3f(0, 0, 1);
		glVertex3f(0, 0, 0);
		glVertex3f(0, 0, 0.18f);
	}
	glEnd();

	// reenable lighting
	glEnable(GL_LIGHTING);
	glEnable(GL_FOG);
}

void SceneManager::drawObject(MeshObject* obj)
{
	// if the object isn't a mesh, or it's empty, or it has no triangles, give up
	if (!obj->geometry) return;
	if (obj->geometry->triangles == NULL || obj->geometry->vertices == NULL) return;

	if (obj->geometry->material)
	{
		MaterialMode mode = obj->geometry->material->getMode();
		if (mode == MaterialMode::SOLID)
		{
			// solid colour material
			float colour[4] = {
				obj->geometry->material->colour.x,
				obj->geometry->material->colour.y,
				obj->geometry->material->colour.z,
				1.0f
			};
			glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, colour);
			glBindTexture(GL_TEXTURE_2D, 0);
		}
		glMaterialf(GL_FRONT, GL_SHININESS, obj->geometry->material->shininess);
		if (mode == MaterialMode::ALBEDO)
		{
			// textured material
			glBindTexture(GL_TEXTURE_2D, obj->geometry->material->albedo->getID());
			float colour[4] = { 1.0f, 1.0f, 1.0f, 1.0f };
			glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, colour);
		}
		if (obj->geometry->material->is_unlit)
			glDisable(GL_LIGHTING);
	}
	else
	{
		// no material, plain 0.8 value solid material
		float colour[4] = { 0.8f, 0.8f, 0.8f, 1.0f };
		glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, colour);
		glBindTexture(GL_TEXTURE_2D, 0);
	}

	// clear emission
	float colour[4] = { 0.0f, 0.0f, 0.0f, 1.0f };
	glMaterialfv(GL_FRONT, GL_EMISSION, colour);

	// fill from front, draw as lines from back
	glPolygonMode(GL_FRONT, GL_FILL);
	glPolygonMode(GL_BACK, GL_LINE);

	// draw triangle mesh
	glBegin(GL_TRIANGLES);
	for (uint32_t i = 0; i < obj->geometry->trisCount(); i++)
	{
		Vector3 vert = obj->geometry->vertices[obj->geometry->triangles[i]];
		Vector3 normal = obj->geometry->vertex_normals[i];
		Vector2 uv = obj->geometry->uvs[i];

		triangles_drawn_last_frame++;

		glTexCoord2f(uv.x, uv.y);
		glNormal3f(normal.x, normal.y, normal.z);
		glVertex3f(vert.x, vert.y, vert.z);
	}
	glEnd();

	glEnable(GL_LIGHTING);
}

void SceneManager::drawParticle(ParticleObject* obj)
{
	if (!obj->material) return;

	MaterialMode mode = obj->material->getMode();
	if (mode == MaterialMode::SOLID)
	{
		// solid colour material
		float colour[4] = {
			obj->material->colour.x,
			obj->material->colour.y,
			obj->material->colour.z,
			1.0f
		};
		glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, colour);
		glBindTexture(GL_TEXTURE_2D, 0);
	}
	glMaterialf(GL_FRONT, GL_SHININESS, obj->material->shininess);
	if (mode == MaterialMode::ALBEDO)
	{
		// textured material
		glBindTexture(GL_TEXTURE_2D, obj->material->albedo->getID());
		float colour[4] = { 1.0f, 1.0f, 1.0f, 1.0f };
		glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, colour);
	}
	if (obj->material->is_unlit)
	{
		glDisable(GL_LIGHTING);
		glColor3f(obj->material->colour.x, obj->material->colour.y, obj->material->colour.z);
	}

	glPolygonMode(GL_FRONT, GL_FILL);
	glPolygonMode(GL_BACK, GL_FILL);

	glDisable(GL_CULL_FACE);
	
	glBegin(GL_QUADS);
	{
		// TODO: make this always face the camera?
		glTexCoord2f(0, 0);
		glVertex3f(-0.5f, -0.5f, 0);
		glTexCoord2f(1, 0);
		glVertex3f(0.5f, -0.5f, 0);
		glTexCoord2f(1, 1);
		glVertex3f(0.5f, 0.5f, 0);
		glTexCoord2f(0, 1);
		glVertex3f(-0.5f, 0.5f, 0);
	}
	glEnd();

	glEnable(GL_CULL_FACE);
	glEnable(GL_LIGHTING);
}

Vector3 sampleLUT(Vector3* lut, Vector3 colour)
{
	int x_index = min((int)floor(colour.x * 64), 63);
	int y_index = min((int)floor(colour.y * 64), 63);
	int z_index = min((int)floor(colour.z * 64), 63);
	return lut[x_index + (y_index * 64) + (z_index * 64 * 64)];
}

void SceneManager::performPostProcessing(CameraObject* camera)
{
	// if camera is invalid, skip
	if (!camera || skip_postprocess) return;
	// if the post processing buffer doesn't exist, create it
	if (!post_processing_buffer)
		post_processing_buffer = new float[viewport_width * viewport_height * 4];

	// read pixels out of the framebuffer
	glReadPixels(0, 0, viewport_width, viewport_height, GL_RGBA, GL_FLOAT, post_processing_buffer);

	Vector2 viewport_divisor = Vector2{ 2.0f / (float)viewport_width, 2.0f / (float)viewport_height };
	// loop over pixels
	Vector2 uv = { -1.0f, -1.0f };
	unsigned int buffer_index = 0;
	Vector3 in_colour;
	for (int y = 0; y < viewport_height; y++)
	{
		for (int x = 0; x < viewport_width; x++)
		{
			// calculate useful stuff, grab data
			in_colour = *((Vector3*)(post_processing_buffer + buffer_index));

			// actual post-processing code
			in_colour *= 1.0f - (((uv.x * uv.x) + (uv.y * uv.y)) * 0.5f); // vignette
			in_colour = sampleLUT(lut_buffer, in_colour); // LUT

			// set back to buffer
			*((Vector3*)(post_processing_buffer + buffer_index)) = in_colour;

			buffer_index += 4;
			uv.x += viewport_divisor.x;
		}
		uv.x = -1.0f;
		uv.y += viewport_divisor.y;
	}

	// write pixels back to the framebuffer
	if (post_process_texture_id == -1)
		glGenTextures(1, &post_process_texture_id);
	glBindTexture(GL_TEXTURE_2D, post_process_texture_id);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, viewport_width, viewport_height, 0, GL_RGBA, GL_FLOAT, post_processing_buffer);
	
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glOrtho(-1, 1, -1, 1, -1, 1);
	
	glDisable(GL_CULL_FACE);
	glPolygonMode(GL_FRONT, GL_FILL);
	glPolygonMode(GL_BACK, GL_FILL);
	float col[4] =
	{
		0.0f,
		0.0f,
		0.0f,
		0.0f
	};
	glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, col);
	col[0] = 1.0f;
	col[1] = 1.0f;
	col[2] = 1.0f;
	col[3] = 1.0f;
	glMaterialfv(GL_FRONT, GL_EMISSION, col);

	glDisable(GL_FOG);
	glColor3f(1.0f, 1.0f, 1.0f);
	glBegin(GL_TRIANGLES);
	{
		// bottom left
		glTexCoord2f(0.0f, 0.0f);
		glVertex3f(-1.0, -1.0f, 0.5f);

		// bottom right
		glTexCoord2f(1.0f, 0.0f);
		glVertex3f(1.0, -1.0f, 0.5f);

		// top right
		glTexCoord2f(1.0f, 1.0f);
		glVertex3f(1.0, 1.0f, 0.5f);

		// top right
		glTexCoord2f(1.0f, 1.0f);
		glVertex3f(1.0, 1.0f, 0.5f);

		// top left
		glTexCoord2f(0.0f, 1.0f);
		glVertex3f(-1.0, 1.0f, 0.5f);

		// bottom left
		glTexCoord2f(0.0f, 0.0f);
		glVertex3f(-1.0, -1.0f, 0.5f);
	}
	glEnd();

	glEnable(GL_LIGHTING);
	glEnable(GL_FOG);
	glEnable(GL_CULL_FACE);

	//glDrawPixels(viewport_width, viewport_height, GL_RGBA, GL_FLOAT, post_processing_buffer);
}

void SceneManager::updateLights()
{
	for (int light_index = 0; light_index < 8; light_index++)
	{
		GLenum light = GL_LIGHT0 + light_index;
		if (!lights[light_index].enabled)
		{
			glDisable(light);
			continue;
		}
		glEnable(light);

		float tmp_col[4] = { 0.0f };
		tmp_col[0] = lights[light_index].diffuse_colour.x;
		tmp_col[1] = lights[light_index].diffuse_colour.y;
		tmp_col[2] = lights[light_index].diffuse_colour.z;
		tmp_col[3] = 1.0f;
		glLightfv(light, GL_DIFFUSE, tmp_col);
		tmp_col[0] = lights[light_index].specular_colour.x;
		tmp_col[1] = lights[light_index].specular_colour.y;
		tmp_col[2] = lights[light_index].specular_colour.z;
		glLightfv(light, GL_SPECULAR, tmp_col);
		tmp_col[0] = lights[light_index].ambient_colour.x;
		tmp_col[1] = lights[light_index].ambient_colour.y;
		tmp_col[2] = lights[light_index].ambient_colour.z;
		glLightfv(light, GL_SPECULAR, tmp_col);

		if (lights[light_index].type == LightType::DIRECTIONAL)
		{
			tmp_col[0] = -lights[light_index].direction.x;
			tmp_col[1] = -lights[light_index].direction.y;
			tmp_col[2] = -lights[light_index].direction.z;
			tmp_col[3] = 0.0f;
			glLightfv(light, GL_POSITION, tmp_col);
			continue;
		}

		tmp_col[0] = lights[light_index].local_position.x;
		tmp_col[1] = lights[light_index].local_position.y;
		tmp_col[2] = lights[light_index].local_position.z;
		glLightfv(light, GL_POSITION, tmp_col);
		tmp_col[0] = lights[light_index].direction.x;
		tmp_col[1] = lights[light_index].direction.y;
		tmp_col[2] = lights[light_index].direction.z;
		glLightfv(light, GL_SPOT_DIRECTION, tmp_col);
		glLightf(light, GL_SPOT_CUTOFF, lights[light_index].spot_angle);
		glLightf(light, GL_SPOT_EXPONENT, lights[light_index].spot_exponent);
		glLightf(light, GL_CONSTANT_ATTENUATION, lights[light_index].attenuation_constant);
		glLightf(light, GL_LINEAR_ATTENUATION, lights[light_index].attenuation_linear);
		glLightf(light, GL_QUADRATIC_ATTENUATION, lights[light_index].attenuation_quadratic);
	}
}

SceneManager::~SceneManager()
{ }

void SceneManager::addOverlayObject(Object* obj)
{
	if (!obj) return;

	overlay_root->addChild(obj);
}

void SceneManager::togglePostprocess()
{
	skip_postprocess = !skip_postprocess;
}

void SceneManager::menuAction(int index)
{
	switch (index)
	{
	case 1:
		game_manager->destroy();
		setGameManager(nextGame());
		break;
	case 2:
		togglePostprocess();
		break;
	}
}

void SceneManager::setGameManager(GameManager* game)
{
	game_manager = game;
	game_manager->start(this);
}
