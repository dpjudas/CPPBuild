
#include "Precomp.h"
#include "MSIDialog.h"
#include "Base64/Base64.h"

#ifdef WIN32

// These are all garbage - visually obsolete by a couple of decades at this point. Remove them from the dialogs or find some new ones?
static const char* binary_completi = "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";
static const char* binary_custicon = "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";
static const char* binary_exclamic = "AAABAAEAICAQAAAAAADoAgAAFgAAACgAAAAgAAAAQAAAAAEABAAAAAAAgAIAAAAAAAAAAAAAAAAAAAAAAADAwMAAgICAAACAgAAAAAAAAP//AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAMzMxERERERERERERERETMzMyMzMzMzMzMzMzMzMzETMzJEREREREREREREREQDETMkREREREREREREREREQDEzJERERERERAREREREREQxMyREREREREMzREREREREMTMkREREREQDMwREREREQDEzMkREREREQzNEREREREMRMzJEREREREQEREREREQDEzMzJEREREREREREREREMRMzMyRERERERDREREREQDEzMzMyREREREAwREREREMRMzMzMkRERERCMkREREQDEzMzMzMkREREQzNEREREMRMzMzMzJERERAMzBEREQDEzMzMzMzJEREQjMyREREMRMzMzMzMyREREMzM0REQDEzMzMzMzMyRERDMzNEREMRMzMzMzMzMkREQzMzREQDEzMzMzMzMzMkREMzM0REMRMzMzMzMzMzJERDMzNEQDEzMzMzMzMzMzJEQDMwREMRMzMzMzMzMzMyREREREQDEzMzMzMzMzMzMyREREREMRMzMzMzMzMzMzMkREREQDEzMzMzMzMzMzMzMkREREMRMzMzMzMzMzMzMzJEREQDEzMzMzMzMzMzMzMzJEREMRMzMzMzMzMzMzMzMyREQDEzMzMzMzMzMzMzMzMyRAMTMzMzMzMzMzMzMzMzMyIjMzMzMzMzMzMzMzMzMzMzMzMzMzMzMzP4AAAH4AAAA8AAAAGAAAABgAAAAYAAAAGAAAABwAAAAcAAAAPgAAAD4AAAB/AAAAfwAAAP+AAAD/gAAB/8AAAf/AAAP/4AAD/+AAB//wAAf/8AAP//gAD//4AB///AAf//wAP//+AD///gB///8Af///AP///4H////H///////w==";
static const char* binary_info = "AAABAAIAICAQAAAAAADoAgAAJgAAABAQEAAAAAAAKAEAAA4DAAAoAAAAIAAAAEAAAAABAAQAAAAAAIACAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAgAAAgAAAAICAAIAAAACAAIAAgIAAAICAgADAwMAAAAD/AAD/AAAA//8A/wAAAP8A/wD//wAA////AAAAAAAAAAAAAAdwAAAAAAAAAAAAAAAAAAB3cAAAAAAAAAAAAAAAAAAAB3AAAAAAAAAAAAAAAAAADwdwAAAAAAAAAAAAAAAAAP8HcAAAAAAAAAAAAAAABw//B3AAAAAAAAAAAAAAd3cP/wd3dwAAAAAAAAAAd3AAj/8Hd3d3AAAAAAAABwAI////gAB3d3AAAAAAAACP///////4AHd3AAAAAACP//////////gHd3AAAAAP////////////8Hd3AAAA//////////////8Hd3AAf/////zMzMzM////8HdwB4///////MzM//////gHdwf///////zMzP//////B3d4///////8zMz//////4B3f////////MzM///////wd3////////zMzP//////8Hd////////8zMz///////B3f////////MzM///////wd3///////MzMzP//////8HB4/////////////////4BwB/////////////////8HAAeP/////4zMyP/////4AAAAf//////MzMz/////8AAAAAf/////zMzM/////wAAAAAAf////4zMyP////AAAAAAAAeP//////////hwAAAAAAAAd4////////h3AAAAAAAAAAB3eP///4d3AAAAAAAAAAAAAAd3d3dwAAAAAAAA///n////x////4f///8H///+B///+Af//8AA//8AAD/+AAAf/AAAD/gAAAfwAAAD4AAAAcAAAAGAAAAAgAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAQAAAAGAAAADgAAAB8AAAA/gAAAf8AAAP/gAAH/8AAD//wAD///gH/8oAAAAEAAAACAAAAABAAQAAAAAAMAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAgAAAgAAAAICAAIAAAACAAIAAgIAAAICAgADAwMAAAAD/AAD/AAAA//8A/wAAAP8A/wD//wAA////AAAAAAAABwAAAAAABw8HAAAAAAcA/wcAAAAAAI//B3AAAACP//8AdwAAeP////gHcAf///////BwB///zMz/8Hd////8z///B3////zP//8Hf///zM///wd/////////AAf///zP//AAB4//+I//8AAAd4////dwAAAAd3d3cAAA/48AAP4PAAD4DwAA8AcAAOADAADAAQAAgAEAAIAAAAAAAAAAAAAAAAAAAAAAAQAAgAMAAIADAADABwAA8B8AAA==";
static const char* binary_insticon = "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";
static const char* binary_New = "AAABAAEAEBAQAAAAAAAoAQAAFgAAACgAAAAQAAAAIAAAAAEABAAAAAAAgAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAACAAACAAAAAgIAAgAAAAIAAgACAgAAAwMDAAICAgAAAAP8AAP8AAAD//wD/AAAA/wD/AP//AAD///8AAAAAAAAAAAAAAAAAAAAAAAC7u7u7sAAAALu7u7uwAAAAu7u7u7AAAAC7u7u7sAAAALu7u7uwAAAAu7u7u7AAAAAAAAAAAAAAAAu7sAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAD//wAAgA8AAIAPAACADwAAgA8AAIANAACACwAAgAcAAIAaAADA1wAA4asAAP99AAD/7wAA//8AAP/vAAD//wAA";
static const char* binary_removico = "AAABAAIAICAQAAAAAADoAgAAJgAAACAgAAAAAAAAqAgAAA4DAAAoAAAAIAAAAEAAAAABAAQAAAAAAIACAAAAAAAAAAAAABAAAAAAAAAAAAAAAAAAgAAAgAAAAICAAIAAAACAAIAAgIAAAMDAwACAgIAAAAD/AAD/AAAA//8A/wAAAP8A/wD//wAA////AAAAAAAAAAAAAAAAAAAAAAAAAAAH////f3AAAAAAAAAAAAAH9/////f39wAAAAAAAAAId3//////f3d3AAAAAAAACIf3///+d/dmyHcAAAAAAAh3f////od2dmbIdwAAAAAIh/f///7obmZmbId3AAAACHd////+5uaId3zHd4AAAAiH9////u7mZ393yHeAAAAId3////7u7uZn93x3gAAACIf3///+7uZnf3d8d4AAAAh3f////u5n9/f3fHeAAAAIh/f///7mf393d3d3gAAACHd////+Z//3dEREd4AAAAiH9////35/f3fMxHeAAAAId3////9+d/d3fMR3gAAACIf3///3/ud3d8bEd4AAAAh3f/////fm5mxsxHeAAAAIh/f///f/fmZmzHR3gAAACHd/////9/d2bGd0d4AAAAiHd3d3d3d3d3d3dHeAAAAId3//////d3d3d3d3gAAACH94RFyIh3/3d3d3d4AAAAj4R3d3dwGBh393B3eAAAAI9GX+/nA7fwGIf3cHgAAACPB2X+cDt/B7exiPcIAAAACBd2X0G38HZ7+3CI8AAAAACB92XCEwfnZ78zcIcAAAAACB98jw0G//Z7NwOHAAAAAACBx/B3s0b/Y/A9gAAAAAAACB8BO9CBRv9DiAAAAAAAAACAiAMIMzRgiIAAAA/gB///gAD//wAAP/4AAA/+AAAD/gAAAP4AAAB+AAAAPgAAAD4AAAA+AAAAPgAAAD4AAAA+AAAAPgAAAD4AAAA+AAAAPgAAAD4AAAA+AAAAPgAAAD4AAAA+AAAAPgAAAD4AAAA+AAAAPwAAAD+AAAA/wAAAP+AAAH/wAAD/+AAD8oAAAAIAAAAEAAAAABAAgAAAAAAIAEAAAAAAAAAAAAAAABAAAAAAAAAAAAAAAAgAAAgAAAAICAAIAAAACAAIAAgIAAAMDAwADA3MAA8MqmANTw/wCx4v8AjtT/AGvG/wBIuP8AJar/AACq/wAAktwAAHq5AABi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";
static const char* binary_repairic = "AAABAAIAICAQAAAAAADoAgAAJgAAACAgAAAAAAAAqAgAAA4DAAAoAAAAIAAAAEAAAAABAAQAAAAAAIACAAAAAAAAAAAAABAAAAAAAAAAAAAAAAAAgAAAgAAAAICAAIAAAACAAIAAgIAAAMDAwACAgIAAAAD/AAD/AAAA//8A/wAAAP8A/wD//wAA////AAAAAAAAAIiAAAAAAAAAAAAAAAAAAIh3u7oAAAAAAAAAAAAAAAj393u6qgAAAAAAAAAAAACHj3+7qqdwAAAAAAAAAAAAiHj3e6p3cAAAAAAAAAL6KIeHj3und+4AAAAAAAAC+ih4eHgAp+7uAAAAAAAAAvooh4eA/w7udwAAAAAAAAL6KHd+4P8IeHgAAAAAAAAC+ih+564Ah4eHB3d3dwAAAvoo7nruv/h4eAAAAHeAAAL6IIeu7r9/h4CIiIh3gAAC+iCK7uu39/hw/////4AAAvog+O67t39/CIiIiIeAAAL6IPmIu7d3AAAAAAAAcAAC+iD5EIiIiHd3d3d3eAAAAv9w+RAI/////////3gAAAAiAfkQCPRERERERE94AAAADwF/kAj053ZmbMxPeAAAAAcAERAI9P53ZmbMT3gAAAAAAIcACPTv53ZmbE94AAAAiICHAAj0/v53ZmZPeACHAHd4APAI9O/v53ZmT3gAj4f3d48ACPR+/v53Zk94AI+I///wAAj0d+/v53ZPeACIAIiIBwAI9Hd+/v53T3gAAAAAAIcACPRERERERE94AAAAAAh3gAj/////////eAAAAAAId4AAiIiIiIiIiIgAAAAACPeAAAAAAAAAAAAAAAAAAAj3gAAAAAAAAAAAAAAAAAAIiIAAAAAAAAAAAAAA//A////AD///gAf//wAD//EAA//gAAH/4AAB/+AAAf/gAAAD4AAAAeAAAADgAAAA4AAAAOAAAADgAAAA4AAAAeAIAAHwCAAB8AgAAfEYAAHxGAABEAgAAQAIAAEAGAABABgAATAYAAH/GAAB/ggAAf4MAAP+D////g////4P//8oAAAAIAAAAEAAAAABAAgAAAAAAIAEAAAAAAAAAAAAAAABAAAAAAAAAAAAAAAAgAAAgAAAAICAAIAAAACAAIAAgIAAAMDAwADA3MAA8MqmANTw/wCx4v8AjtT/AGvG/wBIuP8AJar/AACq/wAAktwAAHq5AABilgAASnMAADJQANTj/wCxx/8Ajqv/AGuP/wBIc/8AJVf/AABV/wAASdwAAD25AAAxlgAAJXMAABlQANTU/wCxsf8Ajo7/AGtr/wBISP8AJSX/AAAA/gAAANwAAAC5AAAAlgAAAHMAAABQAOPU/wDHsf8Aq47/AI9r/wBzSP8AVyX/AFUA/wBJANwAPQC5ADEAlgAlAHMAGQBQAPDU/wDisf8A1I7/AMZr/wC4SP8AqiX/AKoA/wCSANwAegC5AGIAlgBKAHMAMgBQAP/U/wD/sf8A/47/AP9r/wD/SP8A/yX/AP4A/gDcANwAuQC5AJYAlgBzAHMAUABQAP/U8AD/seIA/47UAP9rxgD/SLgA/yWqAP8AqgDcAJIAuQB6AJYAYgBzAEoAUAAyAP/U4wD/sccA/46rAP9rjwD/SHMA/yVXAP8AVQDcAEkAuQA9AJYAMQBzACUAUAAZAP/U1AD/sbEA/46OAP9rawD/SEgA/yUlAP4AAADcAAAAuQAAAJYAAABzAAAAUAAAAP/j1AD/x7EA/6uOAP+PawD/c0gA/1clAP9VAADcSQAAuT0AAJYxAABzJQAAUBkAAP/w1AD/4rEA/9SOAP/GawD/uEgA/6olAP+qAADckgAAuXoAAJZiAABzSgAAUDIAAP//1AD//7EA//+OAP//awD//0gA//8lAP7+AADc3AAAubkAAJaWAABzcwAAUFAAAPD/1ADi/7EA1P+OAMb/awC4/0gAqv8lAKr/AACS3AAAerkAAGKWAABKcwAAMlAAAOP/1ADH/7EAq/+OAI//awBz/0gAV/8lAFX/AABJ3AAAPbkAADGWAAAlcwAAGVAAANT/1ACx/7EAjv+OAGv/awBI/0gAJf8lAAD+AAAA3AAAALkAAACWAAAAcwAAAFAAANT/4wCx/8cAjv+rAGv/jwBI/3MAJf9XAAD/VQAA3EkAALk9AACWMQAAcyUAAFAZANT/8ACx/+IAjv/UAGv/xgBI/7gAJf+qAAD/qgAA3JIAALl6AACWYgAAc0oAAFAyANT//wCx//8Ajv//AGv//wBI//8AJf//AAD+/gAA3NwAALm5AACWlgAAc3MAAFBQAPLy8gDm5uYA2traAM7OzgDCwsIAtra2AKqqqgCenp4AkpKSAIaGhgB6enoAbm5uAGJiYgBWVlYASkpKAD4+PgAyMjIAJiYmABoaGgAODg4A8Pv/AKSgoACAgIAAAAD/AAD/AAAA//8A/wAAAP8A/wD//wAA////AAAAAAAAAAAAAAAAAO/v7/Ly8gAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAO/v5eXYspWJ8vIAAAAAAAAAAAAAAAAAAAAAAAAAAADv5ubl5diylYmXnPIAAAAAAAAAAAAAAAAAAAAAAAAA7+fm5ubl2JWJl5ycnPIAAAAAAAAAAAAAAAAAAMnJyQDt5+fm5uXYiZecnOPj8gAAAAAAAAAAAAAAAAC7vsK67+fn5+fm5eWXnOPj4+Pj8gAAAAAAAAAAAAAAALu+xLrv5efn5+fmAADj4+Pn5+fyAAAAAAAAAAAAAAAAu77Cuu/l5eXn5wD//wDr6+vn5/IAAAAAAAAAAAAAAAC7vsS67+np6enpAP//AOTk6+vr8gAAAAAAAAAAAAAAALu+wrrv4+OcnJWVAADn9//k5+fy5+fn5+fn5wAAAAAAu77Euu/jnJWVienpzef35v/k5PIAAAAAAObn6wAAAAC7vsK6ye+VlYnpqdjN5+f35v/y6+vr6+vr5ufrAAAAALu+xLrJ75WJienY2M3N5/f35vLm5ubm5ubm5+sAAAAAu77CuskX74npqdjNzc3n5/fy6+vr6+vr6+vn6wAAAAC79sS6yRf57e2p2M3Nzefy8gAAAAAAAAAAAADn9QAAALv2wrrJF/kqLerq6urt7ebm5ubm5ubm5ubm6wAAAAAAu/b2vMkX+SotAOv//////////////////+brAAAAAAAAu7vzARf5Ki0A6/8EBAQEBAQEBAQEBAT/5usAAAAAAADw4vMB/xf5LQDr/wT+hYWJiYmJe3t7BP/m6wAAAAAAAPDn8wABAQEAAOv/BI6ThYWJiYmJe3sE/+brAAAAAAAA8PLzAOvm8gAA6/8Ek46ThYWJiYmJewT/5usAAPT09ADs7Oz06+b09ADr/wSOk46ThYWJiYmJBP/m6wAA6eTx9Ojm6Oz09OL0AOv/BJOOk46ThYWJiYkE/+brAADp4unk5Obm6Ozi9AAA6/8EhZOOk46ThYWJiQT/5usAAOn26eni4+Lj4vTyAADr/wSFhZOOk46ThYWJBP/m6wAA6ekAAOnp6en05fIAAOv/BIWFhZOOk47+hYUE/+brAAAAAAAAAAAAAOvk8gAA6/8EBAQEBAQEBAQEBAT/5usAAAAAAAAAAADp5uXr8gDr///////////////////m6wAAAAAAAAAAAOnk5OvyAADt7e3t7e3t7e3t7e3t7e3tAAAAAAAAAAAA6f/l6/IAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAADp4uTr8gAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAOnp6enyAAAAAAAAAAAAAAAAAAAAAAAAAAD/8D///8AP//+AB///AAP/8QAD/+AAAf/gAAH/4AAB/+AAAAPgAAAB4AAAAOAAAADgAAAA4AAAAOAAAADgAAAB4AgAAfAIAAHwCAAB8RgAAfEYAAEQCAABAAgAAQAYAAEAGAABMBgAAf8YAAH+CAAB/gwAA/4P///+D////g///w==";
static const char* binary_Up = "AAABAAEAEBAQAAAAAAAoAQAAFgAAACgAAAAQAAAAIAAAAAEABAAAAAAAgAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAACAAACAAAAAgIAAgAAAAIAAgACAgAAAwMDAAICAgAAAAP8AAP8AAAD//wD/AAAA/wD/AP//AAD///8AAAAAAAAAAAAAAAAAAAAAAAC7u7u7u7uwALu7u7u7u7AAu7sAAAC7sAC7uwu7u7uwALu7C7u7u7AAuwAAC7u7sAC7sAC7u7uwALu7C7u7u7AAu7u7u7u7sAAAAAAAAAAAAAu7uwAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAD//wAAgAAAAIAAAACAAAAAgAAAAIAAAACAAAAAgAAAAIAAAACAAAAAgAAAAIABAADAfwAA4P8AAP//AAD//wAA";

static std::shared_ptr<DataBuffer> createBmpBGRA8(int width, int height, bool banner)
{
	BITMAPV5HEADER bmpheader = {
		.bV5Size = sizeof(BITMAPV5HEADER),
		.bV5Width = width,
		.bV5Height = -height,
		.bV5Planes = 1,
		.bV5BitCount = 32,
		.bV5Compression = BI_BITFIELDS,
		.bV5SizeImage = static_cast<DWORD>(width * height * 4),
		.bV5RedMask = 0x00ff0000,
		.bV5GreenMask = 0x0000ff00,
		.bV5BlueMask = 0x000000ff,
		.bV5AlphaMask = 0xff000000,
		.bV5CSType = LCS_sRGB,
		.bV5Intent = LCS_GM_GRAPHICS
	};

	uint32_t pixelsOffset = 14 + sizeof(BITMAPV5HEADER);
	uint32_t fileSize = pixelsOffset + bmpheader.bV5SizeImage;
	auto bmp = DataBuffer::create(fileSize);

	uint8_t* bmpfileheader = bmp->data<uint8_t>();
	bmpfileheader[0] = 'B';
	bmpfileheader[1] = 'M';
	bmpfileheader[2] = fileSize & 0xff;
	bmpfileheader[3] = (fileSize >> 8) & 0xff;
	bmpfileheader[4] = (fileSize >> 16) & 0xff;
	bmpfileheader[5] = (fileSize >> 24) & 0xff;
	bmpfileheader[6] = 0;
	bmpfileheader[7] = 0;
	bmpfileheader[8] = 0;
	bmpfileheader[9] = 0;
	bmpfileheader[10] = pixelsOffset & 0xff;
	bmpfileheader[11] = (pixelsOffset >> 8) & 0xff;
	bmpfileheader[12] = (pixelsOffset >> 16) & 0xff;
	bmpfileheader[13] = (pixelsOffset >> 24) & 0xff;

	memcpy(bmp->data() + 14, &bmpheader, sizeof(BITMAPV5HEADER));

	uint32_t* pixels = (uint32_t*)(bmp->data() + 14 + sizeof(BITMAPV5HEADER));
	for (int y = 0; y < height; y++)
	{
		uint32_t* line = pixels + y * width;
		for (int x = 0; x < width; x++)
		{
			if (banner)
				line[x] = 0xFFFFFFFF;
			else
				line[x] = x < 165 ? 0xFFB0E0E6 : 0xFFFFFFFF;
		}
	}
	return bmp;
}


static std::shared_ptr<DataBuffer> createBmpPalette(int width, int height, bool banner)
{
	BITMAPV5HEADER bmpheader = {
		.bV5Size = sizeof(BITMAPV5HEADER),
		.bV5Width = width,
		.bV5Height = -height,
		.bV5Planes = 1,
		.bV5BitCount = 8,
		.bV5Compression = BI_RGB,
		.bV5CSType = LCS_sRGB,
		.bV5Intent = LCS_GM_GRAPHICS
	};

	int pitch = (width + 3) / 4 * 4;

	RGBQUAD colors[256] = {};
	colors[0].rgbRed = 176;
	colors[0].rgbGreen = 224;
	colors[0].rgbBlue = 230;
	colors[1].rgbRed = 255;
	colors[1].rgbGreen = 255;
	colors[1].rgbBlue = 255;

	uint32_t paletteOffset = 14 + sizeof(BITMAPV5HEADER);
	uint32_t pixelsOffset = paletteOffset + sizeof(RGBQUAD) * 256;
	uint32_t fileSize = pixelsOffset + pitch * height;

	auto bmp = DataBuffer::create(fileSize);

	uint8_t* bmpfileheader = bmp->data<uint8_t>();
	bmpfileheader[0] = 'B';
	bmpfileheader[1] = 'M';
	bmpfileheader[2] = fileSize & 0xff;
	bmpfileheader[3] = (fileSize >> 8) & 0xff;
	bmpfileheader[4] = (fileSize >> 16) & 0xff;
	bmpfileheader[5] = (fileSize >> 24) & 0xff;
	bmpfileheader[6] = 0;
	bmpfileheader[7] = 0;
	bmpfileheader[8] = 0;
	bmpfileheader[9] = 0;
	bmpfileheader[10] = pixelsOffset & 0xff;
	bmpfileheader[11] = (pixelsOffset >> 8) & 0xff;
	bmpfileheader[12] = (pixelsOffset >> 16) & 0xff;
	bmpfileheader[13] = (pixelsOffset >> 24) & 0xff;

	memcpy(bmp->data() + 14, &bmpheader, sizeof(BITMAPV5HEADER));
	memcpy(bmp->data() + paletteOffset, colors, sizeof(RGBQUAD) * 256);

	uint8_t* pixels = (uint8_t*)(bmp->data() + pixelsOffset);
	for (int y = 0; y < height; y++)
	{
		uint8_t* line = pixels + y * pitch;
		for (int x = 0; x < width; x++)
		{
			if (banner)
				line[x] = 1;
			else
				line[x] = x < 165 ? 0 : 1;
		}
	}
	return bmp;
}

MSIDialogs::MSIDialogs()
{
	binary =
	{
		{.name = "bannrbmp", .data = createBmpPalette(500, 63, true) },
		{.name = "completi", .data = Base64::decode(binary_completi) },
		{.name = "custicon", .data = Base64::decode(binary_custicon) },
		{.name = "dlgbmp", .data = createBmpPalette(503, 314, false) },
		{.name = "exclamic", .data = Base64::decode(binary_exclamic) },
		{.name = "info", .data = Base64::decode(binary_info) },
		{.name = "insticon", .data = Base64::decode(binary_insticon) },
		{.name = "New", .data = Base64::decode(binary_New) },
		{.name = "removico", .data = Base64::decode(binary_removico) },
		{.name = "repairic", .data = Base64::decode(binary_repairic) },
		{.name = "Up", .data = Base64::decode(binary_Up) },
	};

	property =
	{
		{.property = "BannerBitmap", .value = "bannrbmp" },
		{.property = "IAgree", .value = "No" },
		{.property = "AppsShutdownOption", .value = "All" },
		{.property = "ProductID", .value = "none" },
		{.property = "ARPHELPLINK", .value = "https://example.com" },
		{.property = "ButtonText_Back", .value = "< &Back" },
		{.property = "ButtonText_Browse", .value = "Br&owse" },
		{.property = "ButtonText_Cancel", .value = "Cancel" },
		{.property = "ButtonText_Exit", .value = "&Exit" },
		{.property = "ButtonText_Finish", .value = "&Finish" },
		{.property = "ButtonText_Ignore", .value = "&Ignore" },
		{.property = "ButtonText_Install", .value = "&Install" },
		{.property = "ButtonText_Next", .value = "&Next >" },
		{.property = "ButtonText_No", .value = "&No" },
		{.property = "ButtonText_OK", .value = "OK" },
		{.property = "ButtonText_Print", .value = "&Print" },
		{.property = "ButtonText_Remove", .value = "&Remove" },
		{.property = "ButtonText_Repair", .value = "&Repair" },
		{.property = "ButtonText_Reset", .value = "&Reset" },
		{.property = "ButtonText_Resume", .value = "&Resume" },
		{.property = "ButtonText_Retry", .value = "&Retry" },
		{.property = "ButtonText_Return", .value = "&Return" },
		{.property = "ButtonText_Yes", .value = "&Yes" },
		{.property = "CompleteSetupIcon", .value = "completi" },
		{.property = "ComponentDownload", .value = "https://example.com/components/" },
		{.property = "CustomSetupIcon", .value = "custicon" },
		{.property = "DefaultUIFont", .value = "DlgFont8" },
		{.property = "DialogBitmap", .value = "dlgbmp" },
		{.property = "DlgTitleFont", .value = "{&DlgFontBold8}" },
		{.property = "ErrorDialog", .value = "ErrorDlg" },
		{.property = "ExclamationIcon", .value = "exclamic" },
		{.property = "InfoIcon", .value = "info" },
		{.property = "InstallerIcon", .value = "insticon" },
		{.property = "INSTALLLEVEL", .value = "3" },
		{.property = "InstallMode", .value = "Typical" },
		{.property = "Link", .value = "Visit us on example.com!" },
		{.property = "LinkURL", .value = "https://example.com" },
		{.property = "PIDTemplate", .value = "12345<###-%%%%%%%>@@@@@" },
		{.property = "Progress1", .value = "Installing" },
		{.property = "Progress2", .value = "installs" },
		{.property = "PROMPTROLLBACKCOST", .value = "P" },
		{.property = "RemoveIcon", .value = "removico" },
		{.property = "RepairIcon", .value = "repairic" },
		{.property = "Setup", .value = "Setup" },
		{.property = "ShowUserRegistrationDlg", .value = "0" },
		{.property = "Wizard", .value = "Setup Wizard" },
	};

	textStyle =
	{
		{.textStyle = "DlgFont8", .faceName = "Tahoma", .size = 8, .styleBits = 0 },
		{.textStyle = "DlgFontBold8", .faceName = "Tahoma", .size = 8, .styleBits = 1 },
		{.textStyle = "VerdanaBold13", .faceName = "Verdana", .size = 13, .styleBits = 1 },
	};

	uiText =
	{
		{.key = "AbsentPath" },
		{.key = "bytes", .text = "bytes" },
		{.key = "GB", .text = "GB" },
		{.key = "KB", .text = "KB" },
		{.key = "MB", .text = "MB" },
		{.key = "MenuAbsent", .text = "Entire feature will be unavailable" },
		{.key = "MenuAdvertise", .text = "Feature will be installed when required" },
		{.key = "MenuAllCD", .text = "Entire feature will be installed to run from CD" },
		{.key = "MenuAllLocal", .text = "Entire feature will be installed on local hard drive" },
		{.key = "MenuAllNetwork", .text = "Entire feature will be installed to run from network" },
		{.key = "MenuCD", .text = "Will be installed to run from CD" },
		{.key = "MenuLocal", .text = "Will be installed on local hard drive" },
		{.key = "MenuNetwork", .text = "Will be installed to run from network" },
		{.key = "ScriptInProgress", .text = "Gathering required information..." },
		{.key = "SelAbsentAbsent", .text = "This feature will remain uninstalled" },
		{.key = "SelAbsentAdvertise", .text = "This feature will be set to be installed when required" },
		{.key = "SelAbsentCD", .text = "This feature will be installed to run from CD" },
		{.key = "SelAbsentLocal", .text = "This feature will be installed on the local hard drive" },
		{.key = "SelAbsentNetwork", .text = "This feature will be installed to run from the network" },
		{.key = "SelAdvertiseAbsent", .text = "This feature will become unavailable" },
		{.key = "SelAdvertiseAdvertise", .text = "Will be installed when required" },
		{.key = "SelAdvertiseCD", .text = "This feature will be available to run from CD" },
		{.key = "SelAdvertiseLocal", .text = "This feature will be installed on your local hard drive" },
		{.key = "SelAdvertiseNetwork", .text = "This feature will be available to run from the network" },
		{.key = "SelCDAbsent", .text = "This feature will be uninstalled completely, you won't be able to run it from CD" },
		{.key = "SelCDAdvertise", .text = "This feature will change from run from CD state to set to be installed when required" },
		{.key = "SelCDCD", .text = "This feature will remain to be run from CD" },
		{.key = "SelCDLocal", .text = "This feature will change from run from CD state to be installed on the local hard drive" },
		{.key = "SelChildCostNeg", .text = "This feature frees up [1] on your hard drive." },
		{.key = "SelChildCostPos", .text = "This feature requires [1] on your hard drive." },
		{.key = "SelCostPending", .text = "Compiling cost for this feature..." },
		{.key = "SelLocalAbsent", .text = "This feature will be completely removed" },
		{.key = "SelLocalAdvertise", .text = "This feature will be removed from your local hard drive, but will be set to be installed when required" },
		{.key = "SelLocalCD", .text = "This feature will be removed from your local hard drive, but will be still available to run from CD" },
		{.key = "SelLocalLocal", .text = "This feature will remain on you local hard drive" },
		{.key = "SelLocalNetwork", .text = "This feature will be removed from your local hard drive, but will be still available to run from the network" },
		{.key = "SelNetworkAbsent", .text = "This feature will be uninstalled completely, you won't be able to run it from the network" },
		{.key = "SelNetworkAdvertise", .text = "This feature will change from run from network state to set to be installed when required" },
		{.key = "SelNetworkLocal", .text = "This feature will change from run from network state to be installed on the local hard drive" },
		{.key = "SelNetworkNetwork", .text = "This feature will remain to be run from the network" },
		{.key = "SelParentCostNegNeg", .text = "This feature frees up [1] on your hard drive. It has [2] of [3] subfeatures selected. The subfeatures free up [4] on your hard drive." },
		{.key = "SelParentCostNegPos", .text = "This feature frees up [1] on your hard drive. It has [2] of [3] subfeatures selected. The subfeatures require [4] on your hard drive." },
		{.key = "SelParentCostPosNeg", .text = "This feature requires [1] on your hard drive. It has [2] of [3] subfeatures selected. The subfeatures free up [4] on your hard drive." },
		{.key = "SelParentCostPosPos", .text = "This feature requires [1] on your hard drive. It has [2] of [3] subfeatures selected. The subfeatures require [4] on your hard drive." },
		{.key = "TimeRemaining", .text = "Time remaining: {[1] minutes }{[2] seconds}" },
		{.key = "VolumeCostAvailable", .text = "Available" },
		{.key = "VolumeCostDifference", .text = "Difference" },
		{.key = "VolumeCostRequired", .text = "Required" },
		{.key = "VolumeCostSize", .text = "Disk Size" },
		{.key = "VolumeCostVolume", .text = "Volume" },
	};

	error =
	{
		{.error = 0, .message = "{{Fatal error: }}" },
		{.error = 1, .message = "{{Error [1]. }}" },
		{.error = 2, .message = "Warning [1]. " },
		{.error = 3 },
		{.error = 4, .message = "Info [1]. " },
		{.error = 5, .message = "The installer has encountered an unexpected error installing this package. This may indicate a problem with this package. The error code is [1]. {{The arguments are: [2], [3], [4]}}" },
		{.error = 6 },
		{.error = 7, .message = "{{Disk full: }}" },
		{.error = 8, .message = "Action [Time]: [1]. [2]" },
		{.error = 9, .message = "[ProductName]" },
		{.error = 10, .message = "{[2]}{, [3]}{, [4]}" },
		{.error = 11, .message = "Message type: [1], Argument: [2]{, [3]}" },
		{.error = 12, .message = "=== Logging started: [Date]  [Time] ===" },
		{.error = 13, .message = "=== Logging stopped: [Date]  [Time] ===" },
		{.error = 14, .message = "Action start [Time]: [1]." },
		{.error = 15, .message = "Action ended [Time]: [1]. Return value [2]." },
		{.error = 16, .message = "Time remaining: {[1] minutes }{[2] seconds}" },
		{.error = 17, .message = "Out of memory. Shut down other applications before retrying." },
		{.error = 18, .message = "Installer is no longer responding." },
		{.error = 19, .message = "Installer stopped prematurely." },
		{.error = 20, .message = "Please wait while Windows configures [ProductName]" },
		{.error = 21, .message = "Gathering required information..." },
		{.error = 22, .message = "Removing older versions of this application..." },
		{.error = 23, .message = "Preparing to remove older versions of this application..." },
		{.error = 32, .message = "{[ProductName] }Setup completed successfully." },
		{.error = 33, .message = "{[ProductName] }Setup failed." },
		{.error = 1101, .message = "Error reading from file: [2]. {{ System error [3].}}  Verify that the file exists and that you can access it." },
		{.error = 1301, .message = "Cannot create the file '[2]'.  A directory with this name already exists.  Cancel the install and try installing to a different location." },
		{.error = 1302, .message = "Please insert the disk: [2]" },
		{.error = 1303, .message = "The installer has insufficient privileges to access this directory: [2].  The installation cannot continue.  Log on as administrator or contact your system administrator." },
		{.error = 1304, .message = "Error writing to file: [2].  Verify that you have access to that directory." },
		{.error = 1305, .message = "Error reading from file [2]. {{ System error [3].}} Verify that the file exists and that you can access it." },
		{.error = 1306, .message = "Another application has exclusive access to the file '[2]'.  Please shut down all other applications, then click Retry." },
		{.error = 1307, .message = "There is not enough disk space to install this file: [2].  Free some disk space and click Retry, or click Cancel to exit." },
		{.error = 1308, .message = "Source file not found: [2].  Verify that the file exists and that you can access it." },
		{.error = 1309, .message = "Error reading from file: [3]. {{ System error [2].}}  Verify that the file exists and that you can access it." },
		{.error = 1310, .message = "Error writing to file: [3]. {{ System error [2].}}  Verify that you have access to that directory." },
		{.error = 1311, .message = "Source file not found{{(cabinet)}}: [2].  Verify that the file exists and that you can access it." },
		{.error = 1312, .message = "Cannot create the directory '[2]'.  A file with this name already exists.  Please rename or remove the file and click retry, or click Cancel to exit." },
		{.error = 1313, .message = "The volume [2] is currently unavailable.  Please select another." },
		{.error = 1314, .message = "The specified path '[2]' is unavailable." },
		{.error = 1315, .message = "Unable to write to the specified folder: [2]." },
		{.error = 1316, .message = "A network error occurred while attempting to read from the file: [2]" },
		{.error = 1317, .message = "An error occurred while attempting to create the directory: [2]" },
		{.error = 1318, .message = "A network error occurred while attempting to create the directory: [2]" },
		{.error = 1319, .message = "A network error occurred while attempting to open the source file cabinet: [2]" },
		{.error = 1320, .message = "The specified path is too long: [2]" },
		{.error = 1321, .message = "The Installer has insufficient privileges to modify this file: [2]." },
		{.error = 1322, .message = "A portion of the folder path '[2]' is invalid.  It is either empty or exceeds the length allowed by the system." },
		{.error = 1323, .message = "The folder path '[2]' contains words that are not valid in folder paths." },
		{.error = 1324, .message = "The folder path '[2]' contains an invalid character." },
		{.error = 1325, .message = "'[2]' is not a valid short file name." },
		{.error = 1326, .message = "Error getting file security: [3] GetLastError: [2]" },
		{.error = 1327, .message = "Invalid Drive: [2]" },
		{.error = 1328, .message = "Error applying patch to file [2].  It has probably been updated by other means, and can no longer be modified by this patch.  For more information contact your patch vendor.  {{System Error: [3]}}" },
		{.error = 1329, .message = "A file that is required cannot be installed because the cabinet file [2] is not digitally signed.  This may indicate that the cabinet file is corrupt." },
		{.error = 1330, .message = "A file that is required cannot be installed because the cabinet file [2] has an invalid digital signature.  This may indicate that the cabinet file is corrupt.{{  Error [3] was returned by WinVerifyTrust.}}" },
		{.error = 1331, .message = "Failed to correctly copy [2] file: CRC error." },
		{.error = 1332, .message = "Failed to correctly move [2] file: CRC error." },
		{.error = 1333, .message = "Failed to correctly patch [2] file: CRC error." },
		{.error = 1334, .message = "The file '[2]' cannot be installed because the file cannot be found in cabinet file '[3]'. This could indicate a network error, an error reading from the CD-ROM, or a problem with this package." },
		{.error = 1335, .message = "The cabinet file '[2]' required for this installation is corrupt and cannot be used. This could indicate a network error, an error reading from the CD-ROM, or a problem with this package." },
		{.error = 1336, .message = "There was an error creating a temporary file that is needed to complete this installation.{{  Folder: [3]. System error code: [2]}}" },
		{.error = 1401, .message = "Could not create key: [2]. {{ System error [3].}}  Verify that you have sufficient access to that key, or contact your support personnel. " },
		{.error = 1402, .message = "Could not open key: [2]. {{ System error [3].}}  Verify that you have sufficient access to that key, or contact your support personnel. " },
		{.error = 1403, .message = "Could not delete value [2] from key [3]. {{ System error [4].}}  Verify that you have sufficient access to that key, or contact your support personnel. " },
		{.error = 1404, .message = "Could not delete key [2]. {{ System error [3].}}  Verify that you have sufficient access to that key, or contact your support personnel. " },
		{.error = 1405, .message = "Could not read value [2] from key [3]. {{ System error [4].}}  Verify that you have sufficient access to that key, or contact your support personnel. " },
		{.error = 1406, .message = "Could not write value [2] to key [3]. {{ System error [4].}}  Verify that you have sufficient access to that key, or contact your support personnel." },
		{.error = 1407, .message = "Could not get value names for key [2]. {{ System error [3].}}  Verify that you have sufficient access to that key, or contact your support personnel." },
		{.error = 1408, .message = "Could not get sub key names for key [2]. {{ System error [3].}}  Verify that you have sufficient access to that key, or contact your support personnel." },
		{.error = 1409, .message = "Could not read security information for key [2]. {{ System error [3].}}  Verify that you have sufficient access to that key, or contact your support personnel." },
		{.error = 1410, .message = "Could not increase the available registry space. [2] KB of free registry space is required for the installation of this application." },
		{.error = 1500, .message = "Another installation is in progress. You must complete that installation before continuing this one." },
		{.error = 1501, .message = "Error accessing secured data. Please make sure the Windows Installer is configured properly and try the install again." },
		{.error = 1502, .message = "User '[2]' has previously initiated an install for product '[3]'.  That user will need to run that install again before they can use that product.  Your current install will now continue." },
		{.error = 1503, .message = "User '[2]' has previously initiated an install for product '[3]'.  That user will need to run that install again before they can use that product." },
		{.error = 1601, .message = "Out of disk space -- Volume: '[2]'; required space: [3] KB; available space: [4] KB.  Free some disk space and retry." },
		{.error = 1602, .message = "Are you sure you want to cancel?" },
		{.error = 1603, .message = "The file [2][3] is being held in use{ by the following process: Name: [4], Id: [5], Window Title: '[6]'}.  Close that application and retry." },
		{.error = 1604, .message = "The product '[2]' is already installed, preventing the installation of this product.  The two products are incompatible." },
		{.error = 1605, .message = "There is not enough disk space on the volume '[2]' to continue the install with recovery enabled. [3] KB are required, but only [4] KB are available. Click Ignore to continue the install without saving recovery information, click Retry to check for available space again, or click Cancel to quit the installation." },
		{.error = 1606, .message = "Could not access network location [2]." },
		{.error = 1607, .message = "The following applications should be closed before continuing the install:" },
		{.error = 1608, .message = "Could not find any previously installed compliant products on the machine for installing this product." },
		{.error = 1609, .message = "An error occurred while applying security settings. [2] is not a valid user or group. This could be a problem with the package, or a problem connecting to a domain controller on the network. Check your network connection and click Retry, or Cancel to end the install. {{Unable to locate the user's SID, system error [3]}}" },
		{.error = 1610, .message = "The setup must update files or services that cannot be updated while the system is running. If you choose to continue, a reboot will be required to complete the setup." },
		{.error = 1611, .message = "The setup was unable to automatically close all applications that are using files that need to be updated. If you choose to continue, a reboot will be required to complete the setup." },
		{.error = 1701, .message = "The key [2] is not valid.  Verify that you entered the correct key." },
		{.error = 1702, .message = "The installer must restart your system before configuration of [2] can continue.  Click Yes to restart now or No if you plan to manually restart later." },
		{.error = 1703, .message = "You must restart your system for the configuration changes made to [2] to take effect. Click Yes to restart now or No if you plan to manually restart later." },
		{.error = 1704, .message = "An installation for [2] is currently suspended.  You must undo the changes made by that installation to continue.  Do you want to undo those changes?" },
		{.error = 1705, .message = "A previous installation for this product is in progress.  You must undo the changes made by that installation to continue.  Do you want to undo those changes?" },
		{.error = 1706, .message = "An installation package for the product [2] cannot be found. Try the installation again using a valid copy of the installation package '[3]'." },
		{.error = 1707, .message = "Installation completed successfully." },
		{.error = 1708, .message = "Installation failed." },
		{.error = 1709, .message = "Product: [2] -- [3]" },
		{.error = 1710, .message = "You may either restore your computer to its previous state or continue the install later. Would you like to restore?" },
		{.error = 1711, .message = "An error occurred while writing installation information to disk.  Check to make sure enough disk space is available, and click Retry, or Cancel to end the install." },
		{.error = 1712, .message = "One or more of the files required to restore your computer to its previous state could not be found.  Restoration will not be possible." },
		{.error = 1713, .message = "[2] cannot install one of its required products. Contact your technical support group.  {{System Error: [3].}}" },
		{.error = 1714, .message = "The older version of [2] cannot be removed.  Contact your technical support group.  {{System Error [3].}}" },
		{.error = 1715, .message = "Installed [2]" },
		{.error = 1716, .message = "Configured [2]" },
		{.error = 1717, .message = "Removed [2]" },
		{.error = 1718, .message = "File [2] was rejected by digital signature policy." },
		{.error = 1719, .message = "The Windows Installer Service could not be accessed. This can occur if the Windows Installer is not correctly installed. Contact your support personnel for assistance." },
		{.error = 1720, .message = "There is a problem with this Windows Installer package. A script required for this install to complete could not be run. Contact your support personnel or package vendor.  {{Custom action [2] script error [3], [4]: [5] Line [6], Column [7], [8] }}" },
		{.error = 1721, .message = "There is a problem with this Windows Installer package. A program required for this install to complete could not be run. Contact your support personnel or package vendor. {{Action: [2], location: [3], command: [4] }}" },
		{.error = 1722, .message = "There is a problem with this Windows Installer package. A program run as part of the setup did not finish as expected. Contact your support personnel or package vendor.  {{Action [2], location: [3], command: [4] }}" },
		{.error = 1723, .message = "There is a problem with this Windows Installer package. A DLL required for this install to complete could not be run. Contact your support personnel or package vendor.  {{Action [2], entry: [3], library: [4] }}" },
		{.error = 1724, .message = "Removal completed successfully." },
		{.error = 1725, .message = "Removal failed." },
		{.error = 1726, .message = "Advertisement completed successfully." },
		{.error = 1727, .message = "Advertisement failed." },
		{.error = 1728, .message = "Configuration completed successfully." },
		{.error = 1729, .message = "Configuration failed." },
		{.error = 1730, .message = "You must be an Administrator to remove this application. To remove this application, you can log on as an Administrator, or contact your technical support group for assistance." },
		{.error = 1731, .message = "The source installation package for the product [2] is out of sync with the client package. Try the installation again using a valid copy of the installation package '[3]'." },
		{.error = 1732, .message = "In order to complete the installation of [2], you must restart the computer. Other users are currently logged on to this computer, and restarting may cause them to lose their work. Do you want to restart now?" },
		{.error = 1733, .message = "The Windows Installer service is not accessible in Safe Mode. Please try again when your computer is not in Safe Mode or you can use System Restore to return your machine to a previous good state." },
		{.error = 1801, .message = "The path [2] is not valid.  Please specify a valid path." },
		{.error = 1802, .message = "Out of memory. Shut down other applications before retrying." },
		{.error = 1803, .message = "There is no disk in drive [2]. Please insert one and click Retry, or click Cancel to go back to the previously selected volume." },
		{.error = 1804, .message = "There is no disk in drive [2]. Please insert one and click Retry, or click Cancel to return to the browse dialog and select a different volume." },
		{.error = 1805, .message = "The folder [2] does not exist.  Please enter a path to an existing folder." },
		{.error = 1806, .message = "You have insufficient privileges to read this folder." },
		{.error = 1807, .message = "A valid destination folder for the install could not be determined." },
		{.error = 1901, .message = "Error attempting to read from the source install database: [2]." },
		{.error = 1902, .message = "Scheduling reboot operation: Renaming file [2] to [3]. Must reboot to complete operation." },
		{.error = 1903, .message = "Scheduling reboot operation: Deleting file [2]. Must reboot to complete operation." },
		{.error = 1904, .message = "Module [2] failed to register.  HRESULT [3].  Contact your support personnel." },
		{.error = 1905, .message = "Module [2] failed to unregister.  HRESULT [3].  Contact your support personnel." },
		{.error = 1906, .message = "Failed to cache package [2]. Error: [3]. Contact your support personnel." },
		{.error = 1907, .message = "Could not register font [2].  Verify that you have sufficient permissions to install fonts, and that the system supports this font." },
		{.error = 1908, .message = "Could not unregister font [2]. Verify that you that you have sufficient permissions to remove fonts." },
		{.error = 1909, .message = "Could not create Shortcut [2]. Verify that the destination folder exists and that you can access it." },
		{.error = 1910, .message = "Could not remove Shortcut [2]. Verify that the shortcut file exists and that you can access it." },
		{.error = 1911, .message = "Could not register type library for file [2].  Contact your support personnel." },
		{.error = 1912, .message = "Could not unregister type library for file [2].  Contact your support personnel." },
		{.error = 1913, .message = "Could not update the ini file [2][3].  Verify that the file exists and that you can access it." },
		{.error = 1914, .message = "Could not schedule file [2] to replace file [3] on reboot.  Verify that you have write permissions to file [3]." },
		{.error = 1915, .message = "Error removing ODBC driver manager, ODBC error [2]: [3]. Contact your support personnel." },
		{.error = 1916, .message = "Error installing ODBC driver manager, ODBC error [2]: [3]. Contact your support personnel." },
		{.error = 1917, .message = "Error removing ODBC driver: [4], ODBC error [2]: [3]. Verify that you have sufficient privileges to remove ODBC drivers." },
		{.error = 1918, .message = "Error installing ODBC driver: [4], ODBC error [2]: [3]. Verify that the file [4] exists and that you can access it." },
		{.error = 1919, .message = "Error configuring ODBC data source: [4], ODBC error [2]: [3]. Verify that the file [4] exists and that you can access it." },
		{.error = 1920, .message = "Service '[2]' ([3]) failed to start.  Verify that you have sufficient privileges to start system services." },
		{.error = 1921, .message = "Service '[2]' ([3]) could not be stopped.  Verify that you have sufficient privileges to stop system services." },
		{.error = 1922, .message = "Service '[2]' ([3]) could not be deleted.  Verify that you have sufficient privileges to remove system services." },
		{.error = 1923, .message = "Service '[2]' ([3]) could not be installed.  Verify that you have sufficient privileges to install system services." },
		{.error = 1924, .message = "Could not update environment variable '[2]'.  Verify that you have sufficient privileges to modify environment variables." },
		{.error = 1925, .message = "You do not have sufficient privileges to complete this installation for all users of the machine.  Log on as administrator and then retry this installation." },
		{.error = 1926, .message = "Could not set file security for file '[3]'. Error: [2].  Verify that you have sufficient privileges to modify the security permissions for this file." },
		{.error = 1927, .message = "Component Services (COM+ 1.0) are not installed on this computer.  This installation requires Component Services in order to complete successfully.  Component Services are available on Windows 2000." },
		{.error = 1928, .message = "Error registering COM+ Application.  Contact your support personnel for more information." },
		{.error = 1929, .message = "Error unregistering COM+ Application.  Contact your support personnel for more information." },
		{.error = 1930, .message = "The description for service '[2]' ([3]) could not be changed." },
		{.error = 1931, .message = "The Windows Installer service cannot update the system file [2] because the file is protected by Windows.  You may need to update your operating system for this program to work correctly. {{Package version: [3], OS Protected version: [4]}}" },
		{.error = 1932, .message = "The Windows Installer service cannot update the protected Windows file [2]. {{Package version: [3], OS Protected version: [4], SFP Error: [5]}}" },
		{.error = 1933, .message = "The Windows Installer service cannot update one or more protected Windows files. {{SFP Error: [2].  List of protected files:\\r\\n[3]}}" },
		{.error = 1934, .message = "User installations are disabled via policy on the machine." },
		{.error = 1935, .message = "An error occurred during the installation of assembly '[6]'. Please refer to Help and Support for more information. {{HRESULT: [3]. assembly interface: [4], function: [5], component: [2]}}" },
		{.error = 1936, .message = "An error occurred during the installation of assembly '[6]'. The assembly is not strongly named or is not signed with the minimal key length. {{HRESULT: [3]. assembly interface: [4], function: [5], component: [2]}}" },
		{.error = 1937, .message = "An error occurred during the installation of assembly '[6]'. The signature or catalog could not be verified or is not valid. {{HRESULT: [3]. assembly interface: [4], function: [5], component: [2]}}" },
		{.error = 1938, .message = "An error occurred during the installation of assembly '[6]'. One or more modules of the assembly could not be found. {{HRESULT: [3]. assembly interface: [4], function: [5], component: [2]}}" },
	};
}

void MSIDialogs::createTables(MSIDatabase* db, std::vector<MSIProperty>& propertyList)
{
	propertyList.insert(propertyList.end(), property.begin(), property.end());

	std::vector<MSIDialog> dialogs;
	std::vector<MSIControl> controls;
	std::vector<MSIControlCondition> controlCondition;
	std::vector<MSIControlEvent> controlEvent;
	std::vector<MSIEventMapping> eventMapping;

	addDialog(adminWelcomeDlg, dialogs, controls, controlCondition, controlEvent, eventMapping);
	addDialog(exitDialog, dialogs, controls, controlCondition, controlEvent, eventMapping);
	addDialog(fatalError, dialogs, controls, controlCondition, controlEvent, eventMapping);
	addDialog(prepareDlg, dialogs, controls, controlCondition, controlEvent, eventMapping);
	addDialog(progressDlg, dialogs, controls, controlCondition, controlEvent, eventMapping);
	addDialog(userExit, dialogs, controls, controlCondition, controlEvent, eventMapping);
	addDialog(adminBrowseDlg, dialogs, controls, controlCondition, controlEvent, eventMapping);
	addDialog(adminInstallPointDlg, dialogs, controls, controlCondition, controlEvent, eventMapping);
	addDialog(adminRegistrationDlg, dialogs, controls, controlCondition, controlEvent, eventMapping);
	addDialog(browseDlg, dialogs, controls, controlCondition, controlEvent, eventMapping);
	addDialog(cancelDlg, dialogs, controls, controlCondition, controlEvent, eventMapping);
	addDialog(customizeDlg, dialogs, controls, controlCondition, controlEvent, eventMapping);
	addDialog(diskCostDlg, dialogs, controls, controlCondition, controlEvent, eventMapping);
	addDialog(errorDlg, dialogs, controls, controlCondition, controlEvent, eventMapping);
	addDialog(filesInUse, dialogs, controls, controlCondition, controlEvent, eventMapping);
	addDialog(licenseAgreementDlg, dialogs, controls, controlCondition, controlEvent, eventMapping);
	addDialog(maintenanceTypeDlg, dialogs, controls, controlCondition, controlEvent, eventMapping);
	addDialog(maintenanceWelcomeDlg, dialogs, controls, controlCondition, controlEvent, eventMapping);
	addDialog(msiRMFilesInUse, dialogs, controls, controlCondition, controlEvent, eventMapping);
	addDialog(outOfDiskDlg, dialogs, controls, controlCondition, controlEvent, eventMapping);
	addDialog(outOfRbDiskDlg, dialogs, controls, controlCondition, controlEvent, eventMapping);
	addDialog(resumeDlg, dialogs, controls, controlCondition, controlEvent, eventMapping);
	addDialog(setupTypeDlg, dialogs, controls, controlCondition, controlEvent, eventMapping);
	addDialog(userRegistrationDlg, dialogs, controls, controlCondition, controlEvent, eventMapping);
	addDialog(verifyReadyDlg, dialogs, controls, controlCondition, controlEvent, eventMapping);
	addDialog(verifyRemoveDlg, dialogs, controls, controlCondition, controlEvent, eventMapping);
	addDialog(verifyRepairDlg, dialogs, controls, controlCondition, controlEvent, eventMapping);
	addDialog(waitForCostingDlg, dialogs, controls, controlCondition, controlEvent, eventMapping);
	addDialog(welcomeDlg, dialogs, controls, controlCondition, controlEvent, eventMapping);
	addDialog(welcomeDlgLink, dialogs, controls, controlCondition, controlEvent, eventMapping);

	db->createTable(dialogs);
	db->createTable(controls);
	db->createTable(controlCondition);
	db->createTable(controlEvent);
	db->createTable(eventMapping);

	db->createTable(textStyle);
	db->createTable(uiText);
	db->createTable(bbControl);
	db->createTable(billboard);
	db->createTable(checkBox);
	db->createTable(radioButton);
	db->createTable(comboBox);
	db->createTable(listBox);
	db->createTable(listView);
	db->createTable(binary);
}

#endif
