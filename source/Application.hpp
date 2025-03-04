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

		static std::string const dataFilePath;

		GLFWwindow * window;
		ImGuiContext * imGuiContext;

		std::vector <Command> commands;
		std::string workingDirectory;
	};
}