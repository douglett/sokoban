#pragma once
#include <string>
#include <vector>
#include <map>
#include <bitset>
using namespace std;

struct GFX {
	static const uint32_t FONT_DATA[];
	static const int  FONT_W = 4, FONT_H = 8;
	struct Rect  { int x, y, w, h; };
	struct Rectf { double x, y, w, h; };
	struct Image { int w, h; vector<uint32_t> data; };
	struct Sprite {
		int image, visible;
		Rect pos, src, hit, hurt;
	};
	struct Tilemap {
		int x, y, image, visible;
		int tw, th, tsize;
		vector<int> data;
	};

// private:
	map<int, Image>      images;
	map<int, Sprite>     sprites;
	map<int, Tilemap>    tilemaps;
// public:
	Image                screen = {0}, fontface = {0};
	int                  pcounter = 1, flag_hit = 0, flag_hurt = 0;
	vector<int>          collisions_map, collisions_sprite;

	// construct and initialize
	void init(int w, int h) {
		// build screen with blank data
		screen.w = w;
		screen.h = h;
		screen.data.resize(w * h, 0xff000000);

		// build fontface data from 1-bit array
		static const int FONT_IMAGE_WIDTH = 128, FONT_IMAGE_HEIGHT = 64;
		fontface.w = FONT_IMAGE_WIDTH;
		fontface.h = FONT_IMAGE_HEIGHT;
		fontface.data.resize(FONT_IMAGE_WIDTH * FONT_IMAGE_HEIGHT, 0);
		for (uint32_t i = 0; i < (FONT_IMAGE_WIDTH * FONT_IMAGE_HEIGHT) / 32; i++) {
			bitset<32> data{ FONT_DATA[i] };
			for (int j = 0; j < 32; j++)
				fontface.data[ i * 32 + (31 - j) ] = data[j] ? 0xffffffff : 0;  // reverse bit order here
		}

		// erase all previous data and reset
		images = {};
		sprites = {};
		tilemaps = {};
		pcounter = 1;
	}

	// make
	int makeimage(int w, int h) {
		int ptr = pcounter++;
		auto& img = images[ptr] = {0};
		img.w = w;
		img.h = h;
		img.data.resize(w * h, 0xff000000);
		return ptr;
	}
	int makesprite(int w, int h, int image) {
		int ptr = pcounter++;
		auto& spr = sprites[ptr] = {0};
		spr.image = image;
		spr.pos = spr.src = spr.hit = spr.hurt = { 0, 0, w, h };
		spr.visible = true;
		return ptr;
	}
	int makemap(int tw, int th, int tsize, int image) {
		int ptr = pcounter++;
		auto& tmap = tilemaps[ptr] = {0};
		tmap.tw = tw;
		tmap.th = th;
		tmap.tsize = tsize;
		tmap.image = image;
		tmap.visible = true;
		tmap.data.resize(tw * th, 0);
		return ptr;
	}

	// free
	void freeimage(int ptr)  { images.erase(ptr); }
	void freesprite(int ptr) { sprites.erase(ptr); }
	void freemap(int ptr)    { tilemaps.erase(ptr); }

	// get (TODO: null/free error checking)
	Image&   getimage(int ptr)  { return images.at(ptr); }
	Sprite&  getsprite(int ptr) { return sprites.at(ptr); }
	Tilemap& getmap(int ptr)    { return tilemaps.at(ptr); }

	// collision
	inline int collide_rect(const Rect& r1, const Rect& r2) {
		return !( r1.x + r1.w - 1 < r2.x 
				|| r1.x > r2.x + r2.w - 1
				|| r1.y + r1.h - 1 < r2.y 
				|| r1.y > r2.y + r2.h - 1 );
	}
	int collide_sprite(const Rect& rect) {
		collisions_sprite = {};
		for (const auto& [i, sprite] : sprites)
			if (collide_rect(rect, sprite.pos))
				collisions_sprite.push_back(i);
		return collisions_sprite.size();
	}
	int collide_sprite(const Sprite& spr, int xoff=0, int yoff=0) {
		collisions_sprite = {};
		Rect nextpos = { spr.pos.x + xoff, spr.pos.y + yoff, spr.pos.w, spr.pos.h };
		for (const auto& [i, sprite] : sprites)
			if (&spr != &sprite && collide_rect(nextpos, sprite.pos))
				collisions_sprite.push_back(i);
		return collisions_sprite.size();
	}
	inline int mapat(const Tilemap& tmap, int tx, int ty) {
		if (tmap.tsize <= 0)  return 0;
		if (tx < 0 || tx >= tmap.tw || ty < 0 || ty > tmap.th)
			return 0;
		return tmap.data[ ty * tmap.tw + tx ];
	}
	inline int mapatpx(const Tilemap& tmap, int x, int y) {
		if (tmap.tsize <= 0)  return 0;
		return mapat( tmap, x / tmap.tsize, y / tmap.tsize );
	}
	int collide_map(int dx, int dy) {
		for (const auto& [i, tmap] : tilemaps) {
			if (tmap.tsize <= 0)  continue;
			if (mapatpx(tmap, dx, dy) < 0)
				return true;
		}
		return false;
	}
	int collide_map(const Rect& r) {
		return collide_map(r.x, r.y)
			|| collide_map(r.x + r.w - 1, r.y)
			|| collide_map(r.x, r.y + r.h - 1)
			|| collide_map(r.x + r.w - 1, r.y + r.h - 1);
		// collisions_map = {};
		// int coll[] = { r.x, r.y,   r.x+r.w-1, r.y,   r.x, r.y+r.h-1,   r.x+r.w-1, r.y+r.h-1 };
		// for (int i = 0; i < 8; i++)
		// 	if (collide_map( coll[i], coll[i+1] ))
		// 		collide_map.push_back(tileat( coll[i], coll[i+1] ));
		// return collide_map.size();
	}
	int collide_map(const Sprite& spr, int xoff=0, int yoff=0) {
		return collide_map((Rect){ spr.pos.x + xoff, spr.pos.y + yoff, spr.pos.w, spr.pos.h });
	}
	int collide_all(const Sprite& spr, int xoff=0, int yoff=0) {
		return collide_map(spr, xoff, yoff) + collide_sprite(spr, xoff, yoff);
	}

	// single pixel
	inline void px(int col, int dx, int dy) {
		px(screen, col, dx, dy);
	}
	inline void px(Image& dst, int col, int dx, int dy) {
		if (dx >= 0 && dx < dst.w && dy >= 0 && dy < dst.h)
			dst.data[ dy * dst.w + dx ] = col;
	}

	// fill box with solid color
	void fill(uint32_t col) {
		fill(screen, col, { 0, 0, screen.w, screen.h });
	}
	void fill(uint32_t col, Rect drect) {
		fill(screen, col, drect);
	}
	void fill(Image& dst, uint32_t col) {
		fill(dst, col, { 0, 0, dst.w, dst.h });
	}
	void fill(Image& dst, uint32_t col, Rect drect) {
		for (int y = 0; y < drect.h; y++)
			if (drect.y + y >= 0 && drect.y + y < dst.h)
				for (int x = 0; x < drect.w; x++)
					if (drect.x + x >= 0 && drect.x + x < dst.w)
						dst.data[ (drect.y + y) * dst.w + (drect.x + x) ] = col;
	}

	// outline box
	void outline(uint32_t col, Rect drect) {
		outline(screen, col, drect);
	}
	void outline(Image& dst, uint32_t col) {
		outline(dst, col, { 0, 0, dst.w, dst.h });
	}
	void outline(Image& dst, uint32_t col, Rect drect) {
		for (int x = 0; x < drect.w; x++)
			px(dst, col, drect.x + x, drect.y),
			px(dst, col, drect.x + x, drect.y + drect.h - 1);
		for (int y = 0; y < drect.h; y++)
			px(dst, col, drect.x, drect.y + y),
			px(dst, col, drect.x + drect.w - 1, drect.y + y);
	}

	// blit image to destination
	void blit(const Image& src, int dx, int dy) {
		blit(screen, src, dx, dy, { 0, 0, src.w, src.h });
	}
	void blit(const Image& src, int dx, int dy, Rect srect) {
		blit(screen, src, dx, dy, srect);
	}
	void blit(Image& dst, const Image& src, int dx, int dy) {
		blit(dst, src, dx, dy, { 0, 0, src.w, src.h });
	}
	void blit(Image& dst, const Image& src, int dx, int dy, Rect srect) {
		if ((int)dst.data.size() != dst.w * dst.h || (int)src.data.size() != src.w * src.h)
			return;
		uint32_t col = 0;
		for (int y = 0; y < srect.h; y++)
			if (y + dy >= 0 && y + dy < dst.h)
				for (int x = 0; x < srect.w; x++)
					if (x + dx >= 0 && x + dx < dst.w) {
						col = src.data[ (y + srect.y) * src.w + (x + srect.x) ];
						// transparency check
						if ((col >> 24) > 0)
							dst.data[ (dy + y) * dst.w + (dx + x) ] = col;
					}
	}

	// print text to image
	void print(const string& str, int dx, int dy) {
		print(screen, str, dx, dy);
	}
	void print(Image& dst, const string& str, int dx, int dy) {
		static const int FONT_WRAP = 32;
		for (int i = 0; str[i] != 0; i++) {
			Rect srect = {
				( str[i] % FONT_WRAP ) * FONT_W,
				( str[i] / FONT_WRAP ) * FONT_H,
				FONT_W, FONT_H  };
			blit(dst, fontface, (dx + i * FONT_W), dy, srect);
		}
	}

	// draw scene
	void drawscene() {
		// drap maps
		for (auto& [i, tmap] : tilemaps) {
			if (!tmap.visible || tmap.image < 1 || (int)tmap.data.size() != tmap.tw * tmap.th)
				continue;
			Rect src = { 0, 0, tmap.tsize, tmap.tsize };
			for (int y = 0; y < tmap.th; y++)
			for (int x = 0; x < tmap.tw; x++) {
				src.x = abs( tmap.data[ y * tmap.tw + x ] ) * tmap.tsize;
				blit( getimage(tmap.image), tmap.x + x * tmap.tsize, tmap.y + y * tmap.tsize, src );
				if (flag_hit && tmap.data[ y * tmap.tw + x ] < 0)
					outline( 0xffff7700, { tmap.x + x * tmap.tsize, tmap.y + y * tmap.tsize, tmap.tsize, tmap.tsize } );
			}
		}
		// draw sprites
		for (auto& [i, sprite] : sprites) {
			if (!sprite.visible || sprite.image < 1)
				continue;
			sprite.src.w = sprite.pos.w;
			sprite.src.h = sprite.pos.h;
			blit( getimage(sprite.image), sprite.pos.x, sprite.pos.y, sprite.src );
			if (flag_hit)
				outline( 0xffff0000, { sprite.pos.x + sprite.hit.x, sprite.pos.y + sprite.hit.y, sprite.hit.w, sprite.hit.h } );
			if (flag_hurt)
				outline( 0xff00ff00, { sprite.pos.x + sprite.hurt.x, sprite.pos.y + sprite.hurt.y, sprite.hurt.w, sprite.hurt.h } );
		}
	}
};


const uint32_t GFX::FONT_DATA[] = {
	0x06604440, 0xf0ff0774, 0x816a7706, 0x66000000, 0x09fa4440, 0xf0ff455d, 0xc3fab80f, 0xf600006f,
	0x0f9eeae4, 0xb4bba756, 0xe76ab606, 0x6624066f, 0x09feeaee, 0x1a55a45f, 0xff6a7906, 0x66ff0f6f,
	0x09fee4ee, 0x1a554456, 0xe76a3906, 0x66ff86f6, 0x0f944444, 0xb4bbecfb, 0xc3f036ff, 0x6f24f0f6,
	0x06644ee0, 0xf0ff4cf2, 0x816a31f6, 0x660000f6, 0x00000000, 0xf0ff0000, 0x00000e0f, 0x00000000,
	0x04a04a46, 0x28000002, 0x44442e4e, 0x44001084, 0x04aae2a2, 0x44a40002, 0xacaaa8a2, 0xaa44204a,
	0x04ae84a4, 0x82440004, 0xe422ac82, 0xaa444e22, 0x040a4440, 0x82ee0e04, 0xe444e2c4, 0x46008014,
	0x040e2450, 0x82440004, 0xa44222a4, 0xa2004e24, 0x000ae8a0, 0x44a46048, 0xa48a2aa8, 0xaa442040,
	0x04004ae0, 0x28002048, 0x4ee42448, 0x44441084, 0x00000030, 0x00004000, 0x00000000, 0x00080000,
	0x44c4cee4, 0xae2a8a2e, 0xc4c4eaaa, 0xaae60600, 0xaaaaa88a, 0xa42a8eaa, 0xaaaa4aaa, 0xaa248240,
	0xaaa8a888, 0xa42c8eea, 0xaaa84aaa, 0xea4482e0, 0xeec8acce, 0xe42c8eea, 0xcac44aaa, 0x444442a0,
	0xeaa8a88a, 0xa42a8aea, 0x8aa24aae, 0xe4444200, 0x8aaaa88a, 0xa4aa8aaa, 0x8aaa4a4e, 0xa4842200,
	0x6ac4ce84, 0xae4aeaae, 0x84a44e4a, 0xa4e62600, 0x00000000, 0x00000000, 0x02000000, 0x0000000f,
	0x60802020, 0x8428c000, 0x00000000, 0x00024850, 0x40802040, 0x80084000, 0x00004000, 0x000444a0,
	0x2cc46446, 0xcc2a4ac4, 0xc6c6eaaa, 0xaae44404, 0x02aaaaea, 0xa42a4eaa, 0xaaa84aaa, 0xaa280204,
	0x06a8ae4a, 0xa42c4eaa, 0xaa844aae, 0x4a44440a, 0x0aaaa846, 0xa42a4aaa, 0xc6824a4e, 0xa684440a,
	0x06c46642, 0xa4aaeaa4, 0x828c264a, 0xa2e2480e, 0x0000000c, 0x00400000, 0x83000000, 0x0c000000,
	0x4a24a8e0, 0x4a8a48ae, 0x2074a848, 0xaaa44ac2, 0xa04a04e0, 0xa040a444, 0x48aa04a4, 0x0404aaa5,
	0x8a4cccc6, 0x444000aa, 0xe6a44400, 0xaaaa84c4, 0x8aa22228, 0xaaacccaa, 0x86faaaaa, 0xaaa8ceae,
	0x8ae66668, 0xeee444ee, 0xccaaaaaa, 0xaaa884f4, 0xaa8aaaa6, 0x888444aa, 0x8caaaaaa, 0x6aaa8ea4,
	0x46666664, 0x666444aa, 0xe2b44466, 0x2444e498, 0x2000000c, 0x00000000, 0x00000000, 0xc0040000,
	0x22225e64, 0x40044400, 0x8ad444a0, 0x0aa0aa40, 0x4444a0aa, 0x0004405a, 0x257444a0, 0x0aa0aa40,
	0xc00002aa, 0x400ee45a, 0x8ad44ca0, 0xcaaeaac0, 0x2c4aca64, 0x400c24a5, 0x2574c4ae, 0x42a22e4c,
	0x64aaae00, 0x87e2a45a, 0x8ad44caa, 0xcaaae0c4, 0xa4aaaeee, 0xa424e45a, 0x257444aa, 0x4aaa0004,
	0x6446aa00, 0x400e2400, 0x8ad444aa, 0x4aaa0004, 0x00000000, 0x00000000, 0x257444aa, 0x4aaa0004,
	0x4404044a, 0xa0a0a0a4, 0xa00a400a, 0x440f0c3f, 0x4404044a, 0xa0a0a0a4, 0xa00a400a, 0x440f0c3f,
	0x4404047a, 0xbfbfbfbf, 0xaf0a770a, 0xf40f0c3f, 0x7ff7ff4b, 0x88008000, 0xf0ff44ff, 0x4c7f0c3f,
	0x0044047a, 0xfbfbbfbf, 0x0fa077aa, 0xf04ffc30, 0x0044044a, 0x0a0aa0a0, 0x04a004aa, 0x404ffc30,
	0x0044044a, 0x0a0aa0a0, 0x04a004aa, 0x404ffc30, 0x0044044a, 0x0a0aa0a0, 0x04a004aa, 0x404ffc30,
	0x0400f000, 0xe6000200, 0x04820400, 0x4003cc00, 0x0aef9055, 0x49630664, 0xe4442445, 0xa002a200,
	0x0caa475a, 0xa9946a8a, 0x0e28540a, 0xa002a4e0, 0x5a8a2a52, 0xaf929eea, 0xe44444e0, 0x4462aee0,
	0xaa8a4a52, 0xa996fa8a, 0x04824405, 0x000a00e0, 0xac8a9a62, 0x49699c6a, 0xe000444a, 0x000600e0,
	0x588af442, 0xe6f6680a, 0x0eee4800, 0x00040000, 0x08000080, 0x00000000, 0x00004000, 0x00000000,
};