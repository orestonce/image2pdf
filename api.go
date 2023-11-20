package image2pdf

import (
	"bytes"
	"fmt"
	"github.com/signintech/gopdf"
	"image"
	"os"
	"sync"
)

type Status struct {
	IsRunning bool
	Percent   int
}

var gStatus Status
var gRequestStop bool
var gStatusLocker sync.Mutex

func GetStatus() (resp Status) {
	gStatusLocker.Lock()
	resp = gStatus
	gStatusLocker.Unlock()
	return resp
}

func EndConv() {
	gStatusLocker.Lock()
	defer gStatusLocker.Unlock()

	gRequestStop = true
}

func IsRequestStop() bool {
	gStatusLocker.Lock()
	defer gStatusLocker.Unlock()
	return gRequestStop
}

func setPercent(cur int, total int) {
	if total <= 0 {
		return
	}
	gStatusLocker.Lock()
	defer gStatusLocker.Unlock()
	gStatus.Percent = cur * 100 / total
}

func beginRun() bool {
	gStatusLocker.Lock()
	defer gStatusLocker.Unlock()
	if gStatus.IsRunning {
		return false
	}
	gStatus.IsRunning = true
	gRequestStop = false
	return true
}

func endRun() {
	gStatusLocker.Lock()
	gStatus.IsRunning = false
	gStatusLocker.Unlock()
}

type Image2PDF_Req struct {
	FileList []string
	PdfOut   string
}

// 参考: https://github.com/liujianping/images2pdf
func Image2PDF(req Image2PDF_Req) (errMsg string) {
	if !beginRun() {
		return ""
	}
	defer endRun()

	if len(req.FileList) == 0 {
		return "please input image file."
	}

	pdf := gopdf.GoPdf{}
	pdf.Start(gopdf.Config{PageSize: *gopdf.PageSizeA4})
	for idx, inputFile := range req.FileList {
		if IsRequestStop() {
			return "request stop1."
		}
		setPercent(idx, len(req.FileList))

		imgFileData, err := os.ReadFile(inputFile)
		if err != nil {
			return fmt.Sprintf("Error reading %s: %v\n", inputFile, err)
		}
		if IsRequestStop() {
			return "request stop2."
		}

		img, _, err := image.Decode(bytes.NewReader(imgFileData))
		if err != nil {
			return fmt.Sprintf("Error reading %s: %v\n", inputFile, err)
		}
		if IsRequestStop() {
			return "request stop3."
		}

		w, h := gopdf.ImgReactagleToWH(img.Bounds())
		pdf.AddPageWithOption(gopdf.PageOption{
			PageSize: &gopdf.Rect{W: w, H: h},
		})
		if IsRequestStop() {
			return "request stop4."
		}
		if err = pdf.Image(inputFile, 0, 0, nil); err != nil {
			return fmt.Sprintf("failed to add image %s: %w", inputFile, err)
		}
	}
	setPercent(99, 100) // 99%
	if IsRequestStop() {
		return "request stop5."
	}
	if err := pdf.WritePdf(req.PdfOut); err != nil {
		return fmt.Sprintf("failed to write pdf: %w", err)
	}
	setPercent(1, 1) // 100%
	return ""
}
