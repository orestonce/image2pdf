package main

import (
	"github.com/orestonce/go2cpp"
	"image2pdf"
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
	ctx.MustCreateAmd64LibraryInDir("image2pdf-qt")
}
