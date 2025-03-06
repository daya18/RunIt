#pragma once

#include <GLFW/glfw3.h>

namespace runit
{
	class Application
	{
	public:
		Application ();
		~Application ();

		void Run ();

	private:
		struct Command
		{
			std::string name;
			std::string command;
		};

		Command & AddCommand ( std::string const & name, std::string const & command );
		Command & AddCommand ();
		void DeleteCommand ( std::string const & name );
		void CreateDefaultDataFile ();
		void Load ();
		void Save ();
		void ShowCommandEditModal ();
		void ShowMenuBar ();
		void ShowScaleModal ();

		static std::string const dataFilePath;

		GLFWwindow * window;
		ImGuiContext * imGuiContext;

		std::vector <Command> commands;
		std::string workingDirectory;
		Command * activeCommand;
		std::string newCommandName;
		std::string newCommand;
		bool openScaleModal { false };
		float scale{ 1.0f };
	};
}