package main

import (
	"fmt"
	"github.com/orestonce/go2cpp"
	"image2pdf"
	"os"
	"strconv"
)

func main() {
	ctx := go2cpp.NewGo2cppContext(go2cpp.NewGo2cppContext_Req{
		CppBaseName:                 "image2pdf",
		EnableQtClass_RunOnUiThread: true,
		EnableQtClass_Toast:         true,
		NotRemoveImplDotGo:          false,
	})
	ctx.Generate1(image2pdf.Image2PDF)
	ctx.Generate1(image2pdf.GetStatus)
	ctx.Generate1(image2pdf.EndConv)
	ctx.Generate1(image2pdf.IsRequestStop)

	goarch := "amd64"
	buildMode := "c-archive"
	if len(os.Args) == 4 {
		switch os.Args[1] {
		case "create-qt-lib":
			goarch = os.Args[2]
			buildMode = os.Args[3]
		}
	}
	fmt.Println("create lib ", strconv.Quote(goarch), strconv.Quote(buildMode))
	ctx.MustCreateLibrary("image2pdf-qt", goarch, buildMode)
}
