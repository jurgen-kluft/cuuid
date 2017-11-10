package xuuid

import (
	"github.com/jurgen-kluft/xbase/package"
	"github.com/jurgen-kluft/xtime/package"
	"github.com/jurgen-kluft/xcode/denv"
	"github.com/jurgen-kluft/xentry/package"
	"github.com/jurgen-kluft/xunittest/package"
)

// GetPackage returns the package object of 'xuuid'
func GetPackage() *denv.Package {
	// Dependencies
	xunittestpkg := xunittest.GetPackage()
	xentrypkg := xentry.GetPackage()
	xbasepkg := xbase.GetPackage()
	xtimepkg := xtime.GetPackage()

	// The main (xuuid) package
	mainpkg := denv.NewPackage("xuuid")
	mainpkg.AddPackage(xunittestpkg)
	mainpkg.AddPackage(xentrypkg)
	mainpkg.AddPackage(xbasepkg)
	mainpkg.AddPackage(xtimepkg)

	// 'xuuid' library
	mainlib := denv.SetupDefaultCppLibProject("xuuid", "github.com\\jurgen-kluft\\xuuid")
	mainlib.Dependencies = append(mainlib.Dependencies, xbasepkg.GetMainLib())
	mainlib.Dependencies = append(mainlib.Dependencies, xtimepkg.GetMainLib())

	// 'xuuid' unittest project
	maintest := denv.SetupDefaultCppTestProject("xuuid_test", "github.com\\jurgen-kluft\\xuuid")
	maintest.Dependencies = append(maintest.Dependencies, xunittestpkg.GetMainLib())
	maintest.Dependencies = append(maintest.Dependencies, xentrypkg.GetMainLib())
	maintest.Dependencies = append(maintest.Dependencies, xbasepkg.GetMainLib())
	maintest.Dependencies = append(maintest.Dependencies, xtimepkg.GetMainLib())
	maintest.Dependencies = append(maintest.Dependencies, mainlib)

	mainpkg.AddMainLib(mainlib)
	mainpkg.AddUnittest(maintest)
	return mainpkg
}
