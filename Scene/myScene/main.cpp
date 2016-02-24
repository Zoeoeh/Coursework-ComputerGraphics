#include "main.h"

float rho = 0.0f;

SceneManager* myScene;  // pointer to a scene manager!

bool initialise()
{
	double xpos = 0; // create initial vars for mouse position
	double ypos = 0;

	
	// ********************************
	// Set input mode - hide the cursor
	// ********************************
	GLFWwindow* window = renderer::get_window();

	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

	// ******************************
	// Capture initial mouse position
	// ******************************
	
	glfwGetCursorPos(window, &xpos, &ypos);

	myScene = new SceneManager(xpos, ypos); // pass in (copied in constructor)

	// initialise the cameras and store in pointer list

	// static target camera at pos [0]
	myScene->cam = new target_camera();
	myScene->cameraList.push_back(myScene->cam);  // add to list so as to not loose the pointer to the camera

	// create target camera
	myScene->cam->set_position(vec3(50.0f, 10.0f, 50.0f));
	myScene->cam->set_target(vec3(0.0f, 0.0f, 0.0f));
	auto aspect = static_cast<float>(renderer::get_screen_width()) / static_cast<float>(renderer::get_screen_height());
	myScene->cam->set_projection(quarter_pi<float>(), aspect, 2.414f, 1000.0f);


	// free_camera!
	myScene->cam = new free_camera();
	myScene->cameraList.push_back(myScene->cam); // add to list (so can be deleted at end)


	// Set camera properties for free camera (default)
	myScene->cam->set_position(vec3(50.0f, 10.0f, 50.0f));
	myScene->cam->set_target(vec3(0.0f, 0.0f, 0.0f));
	myScene->cam->set_projection(quarter_pi<float>(), aspect, 2.414f, 1000.0f);

	myScene->Create();  // run create method for scene man
	return true;
}

bool load_content()
{

	directional_light* light = myScene->light;  // create local pointer to the scenes light
	

	// CREATE TERRAIN
	geometry terrGeom; // geom to load into
	
	// Load height map
	texture height_map("..\\resources\\textures\\heightmaps\\myHeightMap.png");

	// Generate terrain
	myScene->terr->generate_terrain(terrGeom, height_map, 20, 20, 6.0f);

	// create terrain object

	// Use geometry to create terrain mesh
	myScene->meshes["terr"] = mesh(terrGeom);
	
	vector<texture*> terrTextList;				// local list of textures

	terrTextList.push_back( new texture("..\\resources\\textures\\sand.dds"));
	terrTextList.push_back( new texture("..\\resources\\textures\\oranger.jpg"));
	terrTextList.push_back( new texture("..\\resources\\textures\\rock.dds"));
	terrTextList.push_back( new texture("..\\resources\\textures\\snow.dds"));

	myScene->texList.push_back(terrTextList);

	// Create plane mesh
	myScene->meshes["water"] = mesh(geometry_builder::create_plane(200, 200));

	// Create scene
	myScene->meshes["box"] = mesh(geometry_builder::create_box());
	myScene->meshes["pyramid"] = mesh(geometry_builder::create_pyramid());
	
	myScene->meshes["cylinder"] = mesh(geometry_builder::create_cylinder());

	// Red box
	myScene->materials["box"].set_diffuse(vec4(1.0f, 0.0f, 0.0f, 1.0f));

	// Blue pyramid
	myScene->materials["pyramid"].set_diffuse(vec4(0.0f, 0.0f, 1.0f, 1.0f));

	myScene->materials["cylinder"].set_diffuse(vec4(0.53, 0.45, 0.37, 1.0));

	for (auto &e : myScene->materials)
	{
		e.second.set_emissive(vec4(0.0f, 0.0f, 0.0f, 1.0f));
		e.second.set_specular(vec4(1.0f, 1.0f, 1.0f, 1.0f));
		e.second.set_shininess(2.0f);
	}

	// water needs high spec
	myScene->materials["water"].set_shininess(5.0f);
	myScene->materials["cylinder"].set_shininess(25.0f);

	effect *terr_eff = new effect;
	terr_eff->add_shader("shader.vert", GL_VERTEX_SHADER);
	terr_eff->add_shader("shader.frag", GL_FRAGMENT_SHADER);
	terr_eff->add_shader("..\\resources\\shaders\\parts\\weighted_texture.frag", GL_FRAGMENT_SHADER);
	// Build effect
	terr_eff->build();
	myScene->effectList.push_back(terr_eff);


	myScene->root = new Obj(vec3(0.0f, 0.0f, 0.0f), vec3(1.0f, 0.0f, 0.0f), 0.0f, vec3(15.0f, 15.0f, 15.0f), &myScene->meshes["terr"], &myScene->materials["terr"], terrTextList, terr_eff, light, terrn);

	vector<texture*> objTextList;
	objTextList.push_back(new texture("..\\resources\\textures\\checked.gif"));

	myScene->texList.push_back(objTextList);


	vector<texture*> waterText;
	waterText.push_back(new texture("..\\resources\\textures\\water.jpg"));

	myScene->texList.push_back(waterText);


	vector<texture*> pillarText;
	pillarText.push_back(new texture("..\\resources\\textures\\brick.jpg"));
	pillarText.push_back(new texture("..\\resources\\textures\\brick_normalmap.jpg"));
	myScene->texList.push_back(pillarText);


	effect *water_eff = new effect;
	water_eff->add_shader("..\\resources\\shaders\\water.vert", GL_VERTEX_SHADER);
	water_eff->add_shader("..\\resources\\shaders\\water.frag", GL_FRAGMENT_SHADER);
	water_eff->build();
	myScene->effectList.push_back(water_eff);

	effect *eff = new effect;

	// Load in shaders
	eff->add_shader("..\\resources\\shaders\\phong.vert", GL_VERTEX_SHADER);
	eff->add_shader("..\\resources\\shaders\\phong.frag", GL_FRAGMENT_SHADER);
	// Build effect
	eff->build();
	myScene->effectList.push_back(eff);

	effect *norm_eff = new effect;
	norm_eff->add_shader("normShader.vert", GL_VERTEX_SHADER);
	norm_eff->add_shader("normShader.frag", GL_FRAGMENT_SHADER);
	norm_eff->add_shader("..\\resources\\shaders\\parts\\direction.frag", GL_FRAGMENT_SHADER);
	norm_eff->add_shader("..\\resources\\shaders\\parts\\normal_map.frag", GL_FRAGMENT_SHADER);
	norm_eff->build();
	myScene->effectList.push_back(norm_eff);

	Obj *pillar = new Obj(vec3(-5.0f, 5.0f, 30.0f), vec3(1.0f, 0.0f, 0.0f), 0.0f, vec3(0.5f, 0.5f, 0.5f), &myScene->meshes["cylinder"], &myScene->materials["cylinder"], pillarText, norm_eff, light, object);

	Obj *water = new Obj(vec3(0.0f, 1.0f, 0.0f), vec3(1.0f, 0.0f, 0.0f), 0.0f, vec3(0.1f, 0.1f, 0.1f), &myScene->meshes["water"], &myScene->materials["water"], waterText, water_eff, light, waterObj);

	Obj *box = new Obj(vec3(-25.0f, 5.0f, 30.0f), vec3(1.0f, 0.0f, 0.0f), 0.0f, vec3(0.5f, 0.5f, 0.5f), &myScene->meshes["box"], &myScene->materials["box"], objTextList, eff, light, object);

	Obj *pyra = new Obj(vec3(0.0f, 10.0f, 0.0f), vec3(1.0f, 0.0f, 0.0f), 0.0f, vec3(1.0f, 1.0f, 1.0f), &myScene->meshes["pyramid"], &myScene->materials["pyramid"], objTextList, eff, light, object);

	myScene->root->addChild(box, "box");

	myScene->root->addChild(pillar, "pillar");

	myScene->root->addChild(water, "water");

	box->addChild(pyra, "pyramid");
	myScene->list.push_back(water);
	myScene->list.push_back(myScene->root);
	myScene->list.push_back(box);
	myScene->list.push_back(pyra);
	myScene->list.push_back(pillar);

    // ******************************
    // Create box geometry for skybox
    // ******************************
	myScene->terr->generate_skybox(myScene->meshes["skybox"], myScene->cubemaps["outer"], 1);  // SKY NUMBER ONE
	myScene->terr->generate_skybox(myScene->meshes["skyboxInner"], myScene->cubemaps["inner"], 1);
	
    // *********************
    // Load in skybox effect
    // *********************
	effect *sky_eff = new effect;
	sky_eff->add_shader("..\\resources\\shaders\\skybox.vert", GL_VERTEX_SHADER);
	sky_eff->add_shader("..\\resources\\shaders\\skybox.frag", GL_FRAGMENT_SHADER);

    // Build effect
    sky_eff->build();
	myScene->effectList.push_back(sky_eff);

	myScene->skybx = new Obj(vec3(0.0f, 0.0f, 0.0f), vec3(1.0f, 0.0f, 0.0f), 0.0f, vec3(100.0f, 100.0f, 100.0f), &myScene->meshes["skybox"], &myScene->materials["skybox"], objTextList, sky_eff, light, sky);
	Obj *skybx2 = new Obj(vec3(0.0f, 0.0f, 0.0f), vec3(1.0f, 0.0f, 0.0f), 0.6f, vec3(100.0f, 100.0f, 100.0f), &myScene->meshes["skybox"], &myScene->materials["skybox"], objTextList, sky_eff, light, sky);

	myScene->list.push_back(skybx2);
	myScene->skybx->addChild(skybx2, "rodddot");
	skybx2->addChild(myScene->root, "root");  // not workign
	myScene->list.push_back(myScene->skybx);

    return true;
}

bool update(float delta_time)
{
	// rotation angle increment
	rho += pi<float>() * delta_time * 0.005f;


	if (glfwGetKey(renderer::get_window(), GLFW_KEY_T))    // need to get an enum for camera tyoe
		myScene->cam = myScene->cameraList[0];
	if (glfwGetKey(renderer::get_window(), GLFW_KEY_F))
		myScene->cam = myScene->cameraList[1];

	free_camera* freeCam = NULL;
	freeCam = dynamic_cast<free_camera*>(myScene->cam);

	
	

	if (freeCam)
	{

		GLFWwindow* window = renderer::get_window();

		// The ratio of pixels to rotation - remember the fov
		static double ratio_width = quarter_pi<float>() / static_cast<float>(renderer::get_screen_width());
		static double ratio_height = (quarter_pi<float>() * (static_cast<float>(renderer::get_screen_height()) / static_cast<float>(renderer::get_screen_width()))) / static_cast<float>(renderer::get_screen_height());

		double new_x = 0;
		double new_y = 0;

		glfwGetCursorPos(window, &new_x, &new_y);	// Get the current cursor position

		if (myScene->firstMouse)							 // if first mouse take cursor positons from initalised vars
		{
			myScene->current_x = myScene->initialX;
			myScene->current_y = myScene->initialY;
			myScene->firstMouse = false;
		}

		double delta_x = new_x - myScene->current_x;		 // Calculate delta of cursor positions from last frame
		double delta_y = new_y - myScene->current_y;

		delta_x *= ratio_width;					 // Multiply deltas by ratios - gets actual change in orientation
		delta_y *= -ratio_height;

		freeCam->rotate((float)delta_x, (float)delta_y);  // Rotate cameras by delta :: delta_y - x-axis rotation :: delta_x - y-axis rotation



		if (glfwGetKey(renderer::get_window(), GLFW_KEY_W))
			freeCam->move(vec3(0.0f, 0.0f, 1.0f));
		if (glfwGetKey(renderer::get_window(), GLFW_KEY_A))
			freeCam->move(vec3(-1.0f, 0.0f, 0.0f));
		if (glfwGetKey(renderer::get_window(), GLFW_KEY_D))
			freeCam->move(vec3(1.0f, 0.0f, 0.0f));
		if (glfwGetKey(renderer::get_window(), GLFW_KEY_S))
			freeCam->move(vec3(0.0f, 0.0f, -1.0f));


		glfwGetCursorPos(window, &myScene->current_x, &myScene->current_y);  // update cursor pos
	}

	myScene->cam->update(delta_time);  // update the camera
	
	myScene->skybx->update(NULL);

	//myScene->root->update(NULL);
    return true;
}

bool render()
{

	myScene->skybx->render(myScene->skybx);  // is sky true (enable/disable depth)

	//myScene->root->render(myScene->root);

    return true;
}

void main()
{
    // Create application
    app application;
    // Set methods
    application.set_load_content(load_content);
	application.set_initialise(initialise);
    application.set_update(update);
    application.set_render(render);
    // Run application
    application.run();

	for (uint i = 0; i < myScene->list.size(); ++i)          //// getting a memory leak without this regardless of release method...
		delete myScene->list[i];

	myScene->list.clear();

	// method to free memory and delete pointers
	delete myScene;
	myScene = NULL;
}