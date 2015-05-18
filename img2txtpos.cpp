#include <iostream>
#include <algorithm>
#include <vector>
#include <string>
#include <cstdlib>
#include <tesseract/resultiterator.h>
#include <tesseract/baseapi.h>
#include <leptonica/allheaders.h>

int DELTA_X = 1;
int DELTA_Y = 1;
int LINE_MARGIN = 2;

typedef struct {
  int pageNb;
  int lineNb;
  int x1,y1,x2;
  int y2;
  std::string word;
} TxtPos;

bool compareTxtPos(const TxtPos& a,const TxtPos& b) {
  int dy = a.y2 - b.y2;
  if (std::abs(dy) < DELTA_Y) {
    int dx = a.x1 - a.x1;
    if (std::abs(dx) < DELTA_X) {
      return false;
    }
    return (dx < 0) ? true : false;
  }
  else {
    return (dy < 0) ? true : false;
  }
};

void sortByLine(std::vector<TxtPos>& boxes) {
  std::sort(boxes.begin(),boxes.end(),compareTxtPos);
  
  int lineNb = 0;
  int lineY=boxes.begin()->y2;
  
  for(std::vector<TxtPos>::iterator it=boxes.begin();
      it != boxes.end();
      it++) {
    int y = it->y2;
    int dy = lineY - y ;
    if (std::abs(dy) > LINE_MARGIN) {
      lineNb++;
      lineY = y;
    }
    it->lineNb = lineNb;
  }
}


int main(int argc, char** args) {
  if (argc < 2) {
    std::cerr << "img2txtpos image\n";
    return -1;
  }
  std::string filename = std::string(args[1]);
  Pix *image = pixRead(filename.c_str());
  if (image==NULL) {
    std::cerr << "Unable to read image " << filename << "\n";
    return -1;
  }

  tesseract::TessBaseAPI *api = new tesseract::TessBaseAPI();
  api->Init(NULL, "eng");
  api->SetImage(image);
  api->Recognize(0);
  tesseract::ResultIterator* ri = api->GetIterator();
  tesseract::PageIteratorLevel level = tesseract::RIL_WORD;
  if (ri != 0) {
    std::vector<TxtPos> boxes;
    do {
      const char* word = ri->GetUTF8Text(level);
      //      float conf = ri->Confidence(level);
      int x1, y1, x2, y2;
      ri->BoundingBox(level, &x1, &y1, &x2, &y2);
      TxtPos box;
      box.x1 = x1;
      box.y1 = y1;
      box.x2 = x2;
      box.y2 = y2;
      box.pageNb = 1;
      box.lineNb = -1;
      box.word = word;

      boxes.push_back(box);

      delete[] word;
    } while (ri->Next(level));

    std::string fontName = "";
    std::string fontSize = "-1";
    std::string spaceWidth = "-1";
    sortByLine(boxes);
    for(std::vector<TxtPos>::const_iterator it=boxes.begin();
        it != boxes.end();
        it++) {
      std::cout << it->pageNb << ";" << it->lineNb << ";" << fontName << ";" << fontSize << ";" << spaceWidth << ";" << it->x1 << ";" << it->x2 << ";" << it->y1 << ";" << it->y2 << ";" << it->word << "\n";
    }
  }
  return 0;
}
