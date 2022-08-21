package cuuid

import (
	cbase "github.com/jurgen-kluft/cbase/package"
	"github.com/jurgen-kluft/ccode/denv"
	centry "github.com/jurgen-kluft/centry/package"
	chash "github.com/jurgen-kluft/chash/package"
	crandom "github.com/jurgen-kluft/crandom/package"
	ctime "github.com/jurgen-kluft/ctime/package"
	cunittest "github.com/jurgen-kluft/cunittest/package"
)

// GetPackage returns the package object of 'cuuid'
func GetPackage() *denv.Package {
	// Dependencies
	cunittestpkg := cunittest.GetPackage()
	centrypkg := centry.GetPackage()
	cbasepkg := cbase.GetPackage()
	ctimepkg := ctime.GetPackage()
	chashpkg := chash.GetPackage()
	crandompkg := crandom.GetPackage()

	// The main (cuuid) package
	mainpkg := denv.NewPackage("cuuid")
	mainpkg.AddPackage(cunittestpkg)
	mainpkg.AddPackage(centrypkg)
	mainpkg.AddPackage(cbasepkg)
	mainpkg.AddPackage(ctimepkg)
	mainpkg.AddPackage(chashpkg)
	mainpkg.AddPackage(crandompkg)

	// 'cuuid' library
	mainlib := denv.SetupDefaultCppLibProject("cuuid", "github.com\\jurgen-kluft\\cuuid")
	mainlib.Dependencies = append(mainlib.Dependencies, cbasepkg.GetMainLib())
	mainlib.Dependencies = append(mainlib.Dependencies, ctimepkg.GetMainLib())
	mainlib.Dependencies = append(mainlib.Dependencies, chashpkg.GetMainLib())
	mainlib.Dependencies = append(mainlib.Dependencies, crandompkg.GetMainLib())

	// 'cuuid' unittest project
	maintest := denv.SetupDefaultCppTestProject("cuuid_test", "github.com\\jurgen-kluft\\cuuid")
	maintest.Dependencies = append(maintest.Dependencies, cunittestpkg.GetMainLib())
	maintest.Dependencies = append(maintest.Dependencies, centrypkg.GetMainLib())
	maintest.Dependencies = append(maintest.Dependencies, cbasepkg.GetMainLib())
	maintest.Dependencies = append(maintest.Dependencies, ctimepkg.GetMainLib())
	maintest.Dependencies = append(maintest.Dependencies, chashpkg.GetMainLib())
	maintest.Dependencies = append(maintest.Dependencies, crandompkg.GetMainLib())
	maintest.Dependencies = append(maintest.Dependencies, mainlib)

	mainpkg.AddMainLib(mainlib)
	mainpkg.AddUnittest(maintest)
	return mainpkg
}
