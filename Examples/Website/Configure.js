import { Project } from "cppbuild";

Project.setName("Website");

Project.addConfiguration("Debug");
Project.addConfiguration("Release");

Project.addSubdirectory("Common");
Project.addSubdirectory("Webserver");
Project.addSubdirectory("Website");
