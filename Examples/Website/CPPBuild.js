import { Project } from "cppbuild";

Project.setName("Website");

Project.addConfiguration("Debug");
Project.addConfiguration("Release");

Project.addSubdirectory("Webserver");
Project.addSubdirectory("Website");
