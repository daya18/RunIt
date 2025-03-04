#include "Application.hpp"

#include <imgui_impl_opengl3.h>
#include <imgui_impl_glfw.h>

namespace runit
{
	std::string const Application::dataFilePath { "data.json" };

	Application::Application ()
	{
		glfwInit ();

		window = glfwCreateWindow ( 1280, 720, "RunIt", nullptr, nullptr );
		glfwMakeContextCurrent ( window );

		imGuiContext = ImGui::CreateContext ();
		ImGui::SetCurrentContext ( imGuiContext );

		ImGui_ImplGlfw_InitForOpenGL ( window, true );
		ImGui_ImplOpenGL3_Init ();

		if ( !std::filesystem::exists ( dataFilePath ) )
		{
			nlohmann::json json;

			json["Commands"][0]["Name"] = "Command 1";
			json["Commands"][0]["Command"] = "";

			std::ofstream file { dataFilePath };
			file << std::setw ( 4 ) << json;
		}

		std::ifstream file { dataFilePath };
		nlohmann::json json = nlohmann::json::parse ( file );

		for ( auto const & commandJson : json.at ( "Commands" ) )
			commands.emplace_back ( commandJson.at ( "Name" ), commandJson.at ( "Command" ) );
	}

	Application::~Application ()
	{
		ImGui_ImplOpenGL3_Shutdown ();
		ImGui_ImplGlfw_Shutdown ();
		ImGui::DestroyContext ( imGuiContext );
		glfwDestroyWindow ( window );
		glfwTerminate ();
	}

	void Application::Run ()
	{
		while ( !glfwWindowShouldClose ( window ) )
		{
			glfwPollEvents ();

			glClear ( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );

			ImGui_ImplGlfw_NewFrame ();
			ImGui_ImplOpenGL3_NewFrame ();

			ImGui::NewFrame ();

			ImGui::Begin ( "RunIt" );

			std::string cwd { std::filesystem::current_path ().string () };
			ImGui::InputTextWithHint ( "Working directory", cwd.data (), &workingDirectory );

			ImGui::Spacing ();

			for ( int i = 0; auto const & command : commands )
			{
				if ( ImGui::Button ( command.name.data (), { 200, 50 } ) )
				{
					if ( command.command.empty () )
						return;

					std::thread thread { [ & command ] () { system ( command.command.data () ); } };
					thread.detach ();
				}

				if ( ((i++) + 1) % 3 != 0 )
					ImGui::SameLine ();
			}

			ImGui::End ();

			ImGui::ShowDemoWindow ();

			ImGui::EndFrame ();

			ImGui::Render ();
			ImGui_ImplOpenGL3_RenderDrawData ( ImGui::GetDrawData () );

			glfwSwapBuffers ( window );
		}
	}
}