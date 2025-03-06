#include "Application.hpp"

#include <imgui_impl_opengl3.h>
#include <imgui_impl_glfw.h>

namespace runit
{
	std::string const Application::dataFilePath { "data.json" };

	Application::Application ()
	{
		glfwInit ();

		window = glfwCreateWindow ( 400, 600, "RunIt", nullptr, nullptr );
		glfwMakeContextCurrent ( window );

		imGuiContext = ImGui::CreateContext ();
		ImGui::SetCurrentContext ( imGuiContext );

		ImGui_ImplGlfw_InitForOpenGL ( window, true );
		ImGui_ImplOpenGL3_Init ();

		if ( !std::filesystem::exists ( dataFilePath ) )
			CreateDefaultDataFile ();

		Load ();
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
			openScaleModal = false;
			
			glfwPollEvents ();

			glClear ( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );

			ImGui_ImplGlfw_NewFrame ();
			ImGui_ImplOpenGL3_NewFrame ();

			ImGui::NewFrame ();

			ShowMenuBar ();

			ImGui::SetNextWindowPos ( { 0, ImGui::GetFrameHeight () });

			ImGui::SetNextWindowSize ( { 
				ImGui::GetIO ().DisplaySize.x, 
				ImGui::GetIO ().DisplaySize.y - ImGui::GetFrameHeight () 
			} );
			
			ImGui::Begin ( "RunIt", nullptr, ImGuiWindowFlags_NoDecoration );

			if ( ImGui::IsWindowHovered () && ImGui::IsMouseClicked ( 1 ) ) 
				ImGui::OpenPopup ( "WindowContextMenu" );

			std::string cwd { std::filesystem::current_path ().string () };
			ImGui::InputTextWithHint ( "Working directory", cwd.data (), &workingDirectory );

			ImGui::Spacing ();

			for ( int i = 0; auto & command : commands )
			{
				bool buttonPressed { ImGui::Button ( command.name.data (), { ImGui::GetWindowContentRegionWidth (), 50})};

				if ( ImGui::IsItemHovered () && ImGui::IsMouseClicked ( 1 ) )
				{
					ImGui::OpenPopup ( "ButtonContextMenu" );
					activeCommand = &command;
				}

				if ( buttonPressed )
				{
					if ( ! command.command.empty () )
					{
						std::thread thread { [ & command, this ] () {
							std::string cmd { std::string { "cd " } + workingDirectory + " && " + command.command };
							system ( cmd.data () ); 
						} };

						thread.detach ();
					}
				}

				//if ( ((i++) + 1) % 3 != 0 )
				//	ImGui::SameLine ();
			}
			
			bool openButtonEditPopup { false };

			if ( ImGui::BeginPopupContextItem ( "WindowContextMenu" ) )
			{
				if ( ImGui::MenuItem ( "Add Command" ) )
				{
					activeCommand = &AddCommand ();
					openButtonEditPopup = true;
				}

				ImGui::EndPopup ();
			}

			if ( ImGui::BeginPopupContextItem ( "ButtonContextMenu" ) ) 
			{
				if ( ImGui::MenuItem ( "Edit" ) )
					openButtonEditPopup = true;

				if ( ImGui::MenuItem ( "Delete" ) )
					DeleteCommand ( activeCommand->name );

				ImGui::EndPopup ();
			}
			
			if ( openButtonEditPopup )
			{
				newCommandName = activeCommand->name;
				newCommand = activeCommand->command;

				ImGui::OpenPopup ( "Edit Command" );
			}

			if ( openScaleModal )
				ImGui::OpenPopup("Scale");

			ShowCommandEditModal ();
			ShowScaleModal ();

			ImGui::End ();

			ImGui::EndFrame ();

			ImGui::Render ();
			ImGui_ImplOpenGL3_RenderDrawData ( ImGui::GetDrawData () );

			glfwSwapBuffers ( window );
		}
	}
	
	void Application::ShowCommandEditModal ()
	{
		ImVec2 center = ImGui::GetMainViewport ()->GetCenter ();
		ImGui::SetNextWindowPos ( center, ImGuiCond_Appearing, ImVec2 ( 0.5f, 0.5f ) );

		if ( ImGui::BeginPopupModal ( "Edit Command", nullptr, ImGuiWindowFlags_AlwaysAutoResize ) ) 
		{
			ImGui::InputText ( "Name", & newCommandName );
			ImGui::InputText ( "Command", & newCommand );

			if ( ImGui::Button ( "OK", ImVec2 ( 120, 0 ) ) ) 
			{
				activeCommand->name = newCommandName;
				activeCommand->command = newCommand;
				Save ();
				ImGui::CloseCurrentPopup ();
			}
			
			ImGui::SetItemDefaultFocus ();
			ImGui::SameLine ();

			if ( ImGui::Button ( "Cancel", ImVec2 ( 120, 0 ) ) ) 
			{
				ImGui::CloseCurrentPopup ();
			}

			ImGui::EndPopup ();
		}
	}

	void Application::ShowMenuBar ()
	{
		if ( ImGui::BeginMainMenuBar () )
		{
			if ( ImGui::BeginMenu ( "File" ) )
			{
				if ( ImGui::MenuItem ( "Open", "Ctrl+O" ) )
				{
				}
				if ( ImGui::MenuItem ( "Save", "Ctrl+S" ) )
				{
				}
				if ( ImGui::MenuItem ( "Exit", "Alt+F4" ) )
				{
				}
				ImGui::EndMenu ();
			}

			if ( ImGui::BeginMenu ( "Window" ) )
			{
				if ( ImGui::MenuItem ( "Scale" ) )
					openScaleModal = true;

				ImGui::EndMenu ();
				
				ShowScaleModal ();

			}

			ImGui::EndMainMenuBar ();
		}
	}
	
	void Application::ShowScaleModal ()
	{
		ImVec2 center = ImGui::GetMainViewport ()->GetCenter ();
		ImGui::SetNextWindowPos ( center, ImGuiCond_Appearing, ImVec2 ( 0.5f, 0.5f ) );
		
		if ( ImGui::BeginPopup ( "Scale", ImGuiWindowFlags_AlwaysAutoResize ) )
		{
			if ( ImGui::SliderFloat ( "Scale", &scale, 0.5f, 2.0f ) )
			{
				ImGui::GetIO().FontGlobalScale = scale;
			}

			ImGui::EndPopup ();
		}
	}

	void Application::CreateDefaultDataFile ()
	{
		nlohmann::json json;

		json [ "Commands" ][ 0 ][ "Name" ] = "Command 1";
		json [ "Commands" ][ 0 ][ "Command" ] = "";

		std::ofstream file { dataFilePath };
		file << std::setw ( 4 ) << json;
	}

	void Application::Load ()
	{
		std::ifstream file { dataFilePath };
		nlohmann::json json = nlohmann::json::parse ( file );

		for ( auto const & commandJson : json.at ( "Commands" ) )
			commands.emplace_back ( commandJson.at ( "Name" ), commandJson.at ( "Command" ) );
	}

	void Application::Save ()
	{
		nlohmann::json json;

		json [ "Commands" ] = nlohmann::json::array ();

		for ( auto const & command : commands )
		{
			nlohmann::json commandJson;

			commandJson [ "Name" ] = command.name;
			commandJson [ "Command" ] = command.command;

			json.at ( "Commands" ).push_back ( commandJson );
		}

		std::ofstream file { dataFilePath };
		file << std::setw ( 4 ) << json;
	}

	Application::Command & Application::AddCommand ( std::string const & name, std::string const & command )
	{
		auto & newCommand { commands.emplace_back ( name, command ) };
		Save ();
		return newCommand;
	}
	
	Application::Command & Application::AddCommand ()
	{
		int id { rand () };
		std::string name { "Command " + std::to_string ( id ) };
		auto & command { AddCommand ( name, "" ) };
		return command;
	}

	void Application::DeleteCommand ( std::string const & name )
	{
		std::erase_if ( commands, [ &name ] ( Command const & command )
			{ return command.name == name; } );

		Save ();
	}
}