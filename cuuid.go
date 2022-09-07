package main

import (
	ccode "github.com/jurgen-kluft/ccode"
	cpkg "github.com/jurgen-kluft/cuuid/package"
)

func main() {
	ccode.Init()
	ccode.Generate(cpkg.GetPackage())
}
