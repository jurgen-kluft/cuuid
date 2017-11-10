package main

import (
	"github.com/jurgen-kluft/xcode"
	"github.com/jurgen-kluft/xuuid/package"
)

func main() {
	xcode.Generate(xuuid.GetPackage())
}
