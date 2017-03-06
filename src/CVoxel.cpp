#include <CVoxel.h>
#include <CLargestRect.h>
#include <CFile.h>
#include <iostream>
#include <cstring>
#include <cassert>

namespace {

unsigned int default_palette[256] = {
  0x00000000, 0xffffffff, 0xffccffff, 0xff99ffff, 0xff66ffff, 0xff33ffff, 0xff00ffff, 0xffffccff,
  0xffccccff, 0xff99ccff, 0xff66ccff, 0xff33ccff, 0xff00ccff, 0xffff99ff, 0xffcc99ff, 0xff9999ff,
  0xff6699ff, 0xff3399ff, 0xff0099ff, 0xffff66ff, 0xffcc66ff, 0xff9966ff, 0xff6666ff, 0xff3366ff,
  0xff0066ff, 0xffff33ff, 0xffcc33ff, 0xff9933ff, 0xff6633ff, 0xff3333ff, 0xff0033ff, 0xffff00ff,
  0xffcc00ff, 0xff9900ff, 0xff6600ff, 0xff3300ff, 0xff0000ff, 0xffffffcc, 0xffccffcc, 0xff99ffcc,
  0xff66ffcc, 0xff33ffcc, 0xff00ffcc, 0xffffcccc, 0xffcccccc, 0xff99cccc, 0xff66cccc, 0xff33cccc,
  0xff00cccc, 0xffff99cc, 0xffcc99cc, 0xff9999cc, 0xff6699cc, 0xff3399cc, 0xff0099cc, 0xffff66cc,
  0xffcc66cc, 0xff9966cc, 0xff6666cc, 0xff3366cc, 0xff0066cc, 0xffff33cc, 0xffcc33cc, 0xff9933cc,
  0xff6633cc, 0xff3333cc, 0xff0033cc, 0xffff00cc, 0xffcc00cc, 0xff9900cc, 0xff6600cc, 0xff3300cc,
  0xff0000cc, 0xffffff99, 0xffccff99, 0xff99ff99, 0xff66ff99, 0xff33ff99, 0xff00ff99, 0xffffcc99,
  0xffcccc99, 0xff99cc99, 0xff66cc99, 0xff33cc99, 0xff00cc99, 0xffff9999, 0xffcc9999, 0xff999999,
  0xff669999, 0xff339999, 0xff009999, 0xffff6699, 0xffcc6699, 0xff996699, 0xff666699, 0xff336699,
  0xff006699, 0xffff3399, 0xffcc3399, 0xff993399, 0xff663399, 0xff333399, 0xff003399, 0xffff0099,
  0xffcc0099, 0xff990099, 0xff660099, 0xff330099, 0xff000099, 0xffffff66, 0xffccff66, 0xff99ff66,
  0xff66ff66, 0xff33ff66, 0xff00ff66, 0xffffcc66, 0xffcccc66, 0xff99cc66, 0xff66cc66, 0xff33cc66,
  0xff00cc66, 0xffff9966, 0xffcc9966, 0xff999966, 0xff669966, 0xff339966, 0xff009966, 0xffff6666,
  0xffcc6666, 0xff996666, 0xff666666, 0xff336666, 0xff006666, 0xffff3366, 0xffcc3366, 0xff993366,
  0xff663366, 0xff333366, 0xff003366, 0xffff0066, 0xffcc0066, 0xff990066, 0xff660066, 0xff330066,
  0xff000066, 0xffffff33, 0xffccff33, 0xff99ff33, 0xff66ff33, 0xff33ff33, 0xff00ff33, 0xffffcc33,
  0xffcccc33, 0xff99cc33, 0xff66cc33, 0xff33cc33, 0xff00cc33, 0xffff9933, 0xffcc9933, 0xff999933,
  0xff669933, 0xff339933, 0xff009933, 0xffff6633, 0xffcc6633, 0xff996633, 0xff666633, 0xff336633,
  0xff006633, 0xffff3333, 0xffcc3333, 0xff993333, 0xff663333, 0xff333333, 0xff003333, 0xffff0033,
  0xffcc0033, 0xff990033, 0xff660033, 0xff330033, 0xff000033, 0xffffff00, 0xffccff00, 0xff99ff00,
  0xff66ff00, 0xff33ff00, 0xff00ff00, 0xffffcc00, 0xffcccc00, 0xff99cc00, 0xff66cc00, 0xff33cc00,
  0xff00cc00, 0xffff9900, 0xffcc9900, 0xff999900, 0xff669900, 0xff339900, 0xff009900, 0xffff6600,
  0xffcc6600, 0xff996600, 0xff666600, 0xff336600, 0xff006600, 0xffff3300, 0xffcc3300, 0xff993300,
  0xff663300, 0xff333300, 0xff003300, 0xffff0000, 0xffcc0000, 0xff990000, 0xff660000, 0xff330000,
  0xff0000ee, 0xff0000dd, 0xff0000bb, 0xff0000aa, 0xff000088, 0xff000077, 0xff000055, 0xff000044,
  0xff000022, 0xff000011, 0xff00ee00, 0xff00dd00, 0xff00bb00, 0xff00aa00, 0xff008800, 0xff007700,
  0xff005500, 0xff004400, 0xff002200, 0xff001100, 0xffee0000, 0xffdd0000, 0xffbb0000, 0xffaa0000,
  0xff880000, 0xff770000, 0xff550000, 0xff440000, 0xff220000, 0xff110000, 0xffeeeeee, 0xffdddddd,
  0xffbbbbbb, 0xffaaaaaa, 0xff888888, 0xff777777, 0xff555555, 0xff444444, 0xff222222, 0xff111111
};

}

struct Chunk {
  char id[4] { } ;
  int  chunkBytes { 0 };
  int  childBytes { 0 };

  bool isName(const std::string &name) const {
    assert(name.size() == 4);

    return (strncmp(id, name.c_str(), 4) == 0);
  }

  void print(std::ostream &os) {
    for (int i = 0; i < 4; ++i)
      os << id[i];

    os << ": " << chunkBytes << " " << childBytes;
  }
};

bool
CVoxel::
readVox(const std::string &filename)
{
  CFile file(filename);

  if (! file.isReadable())
    return false;

  file.open(CFile::Mode::READ);

  std::vector<uchar> buffer;

  buffer.resize(4);

  if (! file.read(&buffer[0], 4))
    return false;

  if (strncmp((const char *) &buffer[0], "VOX ", 4) != 0)
    return false;

  int version;

  if (! file.readType(version))
    return false;

  if (version != 150)
    return false;

  Chunk chunk;

  if (! file.readType(chunk))
    return false;

  if (chunk.chunkBytes > 0) {
    buffer.resize(chunk.chunkBytes);

    if (! file.read(&buffer[0], chunk.chunkBytes))
      return false;
  }

  bool rangeSet = false;

  int nleft = chunk.childBytes;

  while (nleft > 0) {
    Chunk chunk;

    if (! file.readType(chunk))
      return false;

    if (isDebug()) {
      chunk.print(std::cerr);
      std::cerr << std::endl;
    }

    nleft -= sizeof(Chunk);

    if      (chunk.isName("SIZE")) {
      assert(chunk.chunkBytes == 12);

      if (! file.readType(size_.x) || ! file.readType(size_.y) || ! file.readType(size_.z))
        return false;
    }
    else if (chunk.isName("XYZI")) {
      assert(chunk.chunkBytes > 4);

      int n;

      if (! file.readType(n))
        return false;

      assert(chunk.chunkBytes == 4*(n + 1));

      for (int i = 0; i < n; ++i) {
        Voxel v;

        if (! file.readType(v.x) || ! file.readType(v.y) ||
            ! file.readType(v.z) || ! file.readType(v.c))
          return false;

        if (! rangeSet) {
          xmin_ = v.x; ymin_ = v.y; zmin_ = v.z;
          xmax_ = v.x; ymax_ = v.y; zmax_ = v.z;

          rangeSet = true;
        }
        else {
          xmin_ = std::min(xmin_, int(v.x));
          ymin_ = std::min(ymin_, int(v.y));
          zmin_ = std::min(zmin_, int(v.z));

          xmax_ = std::max(xmax_, int(v.x));
          ymax_ = std::max(ymax_, int(v.y));
          zmax_ = std::max(zmax_, int(v.z));
        }

        voxels_.push_back(v);
      }
    }
    else if (chunk.isName("RGBA")) {
      assert(chunk.chunkBytes == 4*256);

      Color c;

      colors_.push_back(c);

      for (int i = 0; i < 256; ++i) {
        if (! file.readType(c.r) || ! file.readType(c.g) ||
            ! file.readType(c.b) || ! file.readType(c.a))
          return false;

        colors_.push_back(c);
      }
    }
    else {
      if (chunk.chunkBytes > 0) {
        buffer.resize(chunk.chunkBytes);

        if (! file.read(&buffer[0], chunk.chunkBytes))
          return false;
      }
    }

    nleft -= chunk.chunkBytes;
  }

  //------

  if (colors_.empty()) {
    for (int i = 0; i < 256; ++i) {
      Color c(default_palette[4*i + 0], default_palette[4*i + 1],
              default_palette[4*i + 2], default_palette[4*i + 3]);

      colors_.push_back(c);
    }
  }

  return true;
}

void
CVoxel::
toSliceRects(SliceRects &sliceRects) const
{
  typedef CLargestRect<SlicePoints,int> LargestRect;

  // add slice points (set points per z)
  typedef std::map<int,SlicePoints> ISlicePoints;

  ISlicePoints slicePoints;

  for (const auto &v : voxels()) {
    slicePoints[v.z].setValue(v.x, v.y, v.c);
  }

  // add largest rects per slice
  sliceRects.clear();

  for (int z = 0; z < zmax(); ++z) {
    SlicePoints &points = slicePoints[z];

    LargestRect largestRect(points, xmax() + 1, ymax() + 1);

    for (const auto &c : points.colors()) {
      LargestRect::Rect lrect = largestRect.largestRect(c);

      while (lrect.isValid()) {
        CIBBox2D rect(lrect.left, lrect.top,
                      lrect.left + lrect.width,
                      lrect.top  + lrect.height);

        sliceRects.push_back(SliceRect(rect, z, c));

        for (int y = 0; y < lrect.height; ++y) {
          for (int x = 0; x < lrect.width; ++x) {
            points.setValue(x + lrect.left, y + lrect.top, -1);
          }
        }

        lrect = largestRect.largestRect(c);
      }
    }
  }
}

void
CVoxel::
slicePlane(Dir dir, int value, SliceRects &sliceRects) const
{
  SlicePoints points;

  for (const auto &v : voxels()) {
    int vx, vy, vz;

    if      (dir == Dir::X) {
      vx = v.y; vy = v.z; vz = v.x;
    }
    else if (dir == Dir::Y) {
      vx = v.x; vy = v.z; vz = v.y;
    }
    else if (dir == Dir::Z) {
      vx = v.x; vy = v.y; vz = v.z;
    }

    if (vz != value) continue;

    //---

    points.setValue(vx, vy, v.c);
  }

  //---

  typedef CLargestRect<SlicePoints,int> LargestRect;

  int xs, ys;

  if      (dir == Dir::X) {
    xs = ymax();
    ys = zmax();
  }
  else if (dir == Dir::Y) {
    xs = xmax();
    ys = zmax();
  }
  else if (dir == Dir::Z) {
    xs = xmax();
    ys = ymax();
  }

  LargestRect largestRect(points, xs + 1, ys + 1);

  for (const auto &c : points.colors()) {
    LargestRect::Rect lrect = largestRect.largestRect(c);

    while (lrect.isValid()) {
      CIBBox2D rect(lrect.left, lrect.top,
                    lrect.left + lrect.width,
                    lrect.top  + lrect.height);

      sliceRects.push_back(SliceRect(rect, value, c));

      for (int y = 0; y < lrect.height; ++y) {
        for (int x = 0; x < lrect.width; ++x) {
          points.setValue(x + lrect.left, y + lrect.top, -1);
        }
      }

      lrect = largestRect.largestRect(c);

    }
  }
}

void
CVoxel::
outputRaytrace() const
{
  SliceRects sliceRects;

  toSliceRects(sliceRects);

//double xc = (xmin() + xmax())/2.0;
//double yc = (ymin() + ymax())/2.0;
//double zc = (zmin() + zmax())/2.0;

  double xs = xmax() - xmin();
  double ys = ymax() - ymin();
  double zs = zmax() - zmin();

  double s = std::max(std::max(xs, ys), zs);

  std::cout << "<raytracer scale=\"" << 1.0/s << "\">" << std::endl;

  std::cout << "<camera position=\"2,-2,1\" lookat=\"0.5,0.5,0.5\" up=\"0,0,1\"/>" << std::endl;

  for (const auto &sliceRect : sliceRects) {
    const CIBBox2D &rect = sliceRect.rect;

    double cx = rect.getXMin() - xmin();
    double cy = rect.getYMin() - ymin();
    double cz = sliceRect.z    - zmin();

    double xs = rect.getWidth ();
    double ys = rect.getHeight();
    double zs = 1.0;

    CRGBA c = color(sliceRect.c);

    std::string color = c.getRGB().stringEncode();

    std::cout << "<box " <<
     "size=\"" << xs << "," << ys << "," << zs << "\" " <<
     "translate=\"" << cx << "," << cy << "," << cz << "\" " <<
     "color=\"" << color << "\"/>" << std::endl;
  }

  std::cout << "<light position=\"2,-2,1\" color=\"#e4e4e4\"/>" << std::endl;

  std::cout << "</raytracer>" << std::endl;
}

CRGBA
CVoxel::
color(uchar i) const
{
  const Color &c = colors_[i];

  double r = c.r/255.0;
  double g = c.g/255.0;
  double b = c.b/255.0;
  double a = c.a/255.0;

  return CRGBA(r, g, b, a);
}
