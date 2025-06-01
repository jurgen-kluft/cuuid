package cuuid

import (
	cbase "github.com/jurgen-kluft/cbase/package"
	denv "github.com/jurgen-kluft/ccode/denv"
	ccore "github.com/jurgen-kluft/ccore/package"
	chash "github.com/jurgen-kluft/chash/package"
	crandom "github.com/jurgen-kluft/crandom/package"
	ctime "github.com/jurgen-kluft/ctime/package"
	cunittest "github.com/jurgen-kluft/cunittest/package"
)

// GetPackage returns the package object of 'cuuid'
func GetPackage() *denv.Package {
	// Dependencies
	cunittestpkg := cunittest.GetPackage()
	cbasepkg := cbase.GetPackage()
	ccorepkg := ccore.GetPackage()
	ctimepkg := ctime.GetPackage()
	chashpkg := chash.GetPackage()
	crandompkg := crandom.GetPackage()

	// The main (cuuid) package
	mainpkg := denv.NewPackage("github.com\\jurgen-kluft", "cuuid")
	mainpkg.AddPackage(cunittestpkg)
	mainpkg.AddPackage(cbasepkg)
	mainpkg.AddPackage(ccorepkg)
	mainpkg.AddPackage(ctimepkg)
	mainpkg.AddPackage(chashpkg)
	mainpkg.AddPackage(crandompkg)

	// 'cuuid' library
	mainlib := denv.SetupCppLibProject(mainpkg, "cuuid")
	mainlib.AddDependencies(cbasepkg.GetMainLib()...)
	mainlib.AddDependencies(ccorepkg.GetMainLib()...)
	mainlib.AddDependencies(ctimepkg.GetMainLib()...)
	mainlib.AddDependencies(chashpkg.GetMainLib()...)
	mainlib.AddDependencies(crandompkg.GetMainLib()...)

	// 'cuuid' unittest project
	maintest := denv.SetupCppTestProject(mainpkg, "cuuid_test")
	maintest.AddDependencies(cunittestpkg.GetMainLib()...)
	maintest.AddDependencies(cbasepkg.GetMainLib()...)
	maintest.AddDependencies(ccorepkg.GetMainLib()...)
	maintest.AddDependencies(ctimepkg.GetMainLib()...)
	maintest.AddDependencies(chashpkg.GetMainLib()...)
	maintest.AddDependencies(crandompkg.GetMainLib()...)
	maintest.AddDependency(mainlib)

	mainpkg.AddMainLib(mainlib)
	mainpkg.AddUnittest(maintest)
	return mainpkg
}
