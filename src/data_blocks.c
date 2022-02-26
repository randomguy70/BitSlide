#include "../include/data_blocks.h"

#include "../include/data.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

struct DataBlock *dataToBlocks(struct Data *data, bool dataIsEncrypted)
{
	Byte *array;
	unsigned int arraySize;
	struct DataBlock *block1 = NULL, *block = NULL;
	unsigned int numBlocks = (data->size + sizeof(int)) / BLOCK_DATA_SIZE;
	
	// if the data is encrypted, then i don't need to store its size at the end of the last block, and it should fit exactly in a certain number of blocks
	
	if(dataIsEncrypted)
	{
		printf("data is encrypted\n");
		// data should fit exactly in a number of blocks. otherwise, something's wrong
		
		if(data->size > 0 && data->size % BLOCK_DATA_SIZE != 0)
		{
			printf("Data (%u) seems to have been tampered with\n", data->size);
			return NULL;
		}
		
		numBlocks = data->size / BLOCK_DATA_SIZE;
		struct DataBlock **blockList = malloc(sizeof(struct DataBlock) * numBlocks);
		printf("2");
		block1 = blockList[0];
		
		for(unsigned int i = 0, dataOffset = 0; i < numBlocks; i++, dataOffset += BLOCK_DATA_SIZE)
		{
			printf("writing block %d", i);
			block = blockList[i];
			block->width = BLOCK_WIDTH;
			block->height = BLOCK_HEIGHT;
			block->data = malloc(BLOCK_DATA_SIZE);
			
			copyBytes(block->data, data->ptr + dataOffset, BLOCK_DATA_SIZE);
			
			if(i < numBlocks - 1)
			{
				block->next = blockList[i+1];
			}
			else
			{
				block->next = NULL;
			}
		}
		
		return block1;
	}
	
	if((data->size + sizeof(int)) % BLOCK_DATA_SIZE)
	{
		numBlocks++;
	}

	printf("numBlocks: %d\n", numBlocks);

	block1 = malloc(sizeof(struct DataBlock));
	block = block1;

	// initialise block properties

	printf("initialising blocks\n");

	for(unsigned int i=1; i <= numBlocks; i++)
	{
		block->width = BLOCK_WIDTH;
		block->height = BLOCK_HEIGHT;

		if(i == numBlocks)
		{
			block->next = NULL;
			break;
		}
		else
		{
			block->next = malloc(sizeof(struct DataBlock));
			block = block->next;
		}
	}
	
	// copy data into blocks
	
	printf("copying data into blocks\n");
	
	arraySize = BLOCK_DATA_SIZE * numBlocks;
	array = malloc(arraySize);
	
	copyBytes(array, data->ptr, data->size);
	
	// fill the end of the data with 0's
	
	for(Byte *i = array + data->size; i < array + arraySize - 1 - sizeof(int); i++)
	{
		*i = 0;
	}
	
	// store the size at the end of the data
	
	*((int*) (array + arraySize - 1 - sizeof(int))) = data->size;
	
	block = block1;
	
	// make the blocks' pointers point straight to the data
	
	for(unsigned int i = 0; i < numBlocks; i++)
	{
		block->data = array + (i * BLOCK_DATA_SIZE);

		block = block->next;
	}
	
	return block1;
}

struct Data *blocksToData(struct DataBlock *first, bool dataIsEncrypted)
{
	struct DataBlock *block = first;
	struct Data *data = malloc(sizeof(struct Data));
	const unsigned int blockSize = first->width * first->height;
	unsigned int bytesCopied = 0;
	unsigned int numBlocks = 1;
	unsigned int *sizePtr;
	
	if(dataIsEncrypted == true)
	{
		numBlocks = getNumBlocks(first);
		data->size = numBlocks * BLOCK_DATA_SIZE;
		data->ptr = malloc(data->size);
		struct DataBlock *temp;
		
		for(unsigned int i = 0, dataOffset = 0; i < numBlocks; i++, dataOffset += BLOCK_DATA_SIZE)
		{
			copyBytes(data->ptr + dataOffset, first->data, BLOCK_DATA_SIZE);
			temp = first;
			first = first->next;
			free(temp);
		}
		
		return data;
	}
	
	// get data size (stored in the last 4 bytes of the last block)

	while(block->next != NULL)
	{
		block = block->next;
		numBlocks++;
	}
	sizePtr = (unsigned int*) (block->data + blockSize - 1 - sizeof(unsigned int));
	data->size = *sizePtr;
	data->ptr = malloc(data->size);

	printf("counted %d blocks\n", numBlocks);
	printf("data size: %d\n", data->size);

	// copy the data from every block except the last

	printf("copying blocks into data struct\n");

	block = first;

	for(unsigned int i = 0; i < numBlocks - 1; i++)
	{
		copyBytes(data->ptr + bytesCopied, block->data, blockSize);
		bytesCopied += blockSize;
		block = block->next;
	}

	// copy the data from the last block (minus the last 4 bytes)

	printf("copying last block\n");

	copyBytes(data->ptr + bytesCopied, block->data, blockSize - sizeof(int));

	freeBlocks(first);

	return data;
}

int freeBlocks(struct DataBlock *first)
{
	int i = 1;
	struct DataBlock *temp;

	// since the data in all the data blocks is actually one large block in memory, the first block's data pointer is technically the pointer to all of the data

	free(first->data);

	while(first->next != NULL)
	{
		temp = first->next;
		free(first);
		first = temp;
		i++;
	}

	return i;
}

int shiftCol(struct DataBlock *block, unsigned int col, unsigned int ticks, enum Direction dir)
{
	Byte *tempCol;
	size_t checksum1 = 0, checksum2 = 0;

	if(col < 0 || (dir != SHIFT_UP && dir != SHIFT_DOWN) || block == NULL)
	{
		return 0;
	}
	
	col %= block->width;
	ticks %= block->height;
	
	if(ticks == 0)
	{
		return 1;
	}
	
	tempCol = malloc(block->height - ticks);
	
	for(unsigned int i = 0; i < block->width * block->height; i++)
	{
		checksum1 += block->data[i];
	}

	if(dir == SHIFT_DOWN)
	{
		// save the wrapped bytes

		for(unsigned int i = block->height - ticks, j = 0; i < block->height; i++, j++)
		{
			tempCol[j] = getByte(block, col, i);
		}

		// copy body

		for(unsigned int i = block->height - 1; i > ticks - 1; i--)
		{
			Byte byte = getByte(block, col, i - ticks);
			setByte(byte, block, col, i);
		}

		// copy wrapped bytes

		for(unsigned int i = 0; i < ticks; i++)
		{
			Byte byte = tempCol[i];
			setByte(byte, block, col, i);
		}
	}
	
	else if(dir == SHIFT_UP)
	{
		// save the wrapped bytes

		for(unsigned int i = 0; i < ticks; i++)
		{
			tempCol[i] = getByte(block, col, i);
		}

		// copy body

		for(unsigned int i = 0; i < block->height - ticks; i++)
		{
			Byte byte = getByte(block, col, i + ticks);
			setByte(byte, block, col, i);
		}

		// copy wrapped bytes

		for(unsigned int i = block->height - ticks, j = 0; i < block->height; i++, j++)
		{
			Byte byte = tempCol[j];
			setByte(byte, block, col, i);
		}
	}
	
	// debugging checksum
	
	for(unsigned int i = 0; i < block->width * block->height; i++)
	{
		checksum2 += block->data[i];
	}
	if(checksum1 != checksum2)
	{
		printf("something went wrong in shifting column %u\n", col);
	}
	
	return 1;
}

int shiftRow(struct DataBlock *block, unsigned int row, unsigned int ticks, enum Direction dir)
{	
	Byte *tempRow;
	Byte byte;
	unsigned int i, j;
	size_t checksum1 = 0, checksum2 = 0;

	if(row < 0 || (dir != SHIFT_LEFT && dir != SHIFT_RIGHT))
	{
		return 0;
	}
	
	row %= block->height;
	ticks %= block->width;
	
	if(ticks == 0)
	{
		return 1;
	}
	
	tempRow = malloc(ticks);
	
	// debugging checksum
	
	for(unsigned int i = 0; i < block->width * block->height; i ++)
	{
		checksum1 += block->data[i];
	}
	
	// 0111 0111   0111 0111   0111 0111   0111 0111   0111 0111   0111 0111
	// 0111 0111   0111 0111   0111 0111   0111 0111   0111 0111   0111 0111
	
	if(dir == SHIFT_LEFT)
	{
		// save the wrapped bytes
		
		for(i = 0; i < ticks; i++)
		{
			tempRow[i] = getByte(block, i, row);
		}
		
		// copy body
		
		for(i = 0; i < block->width - ticks; i++)
		{
			byte = getByte(block, i + ticks, row);
			setByte(byte, block, i, row);
		}
		
		// copy wrapped bytes
		
		for(i = block->width - ticks, j = 0; i < block->width; i++, j++)
		{
			byte = tempRow[j];
			setByte(byte, block, i, row);
		}
	}
	
	if(dir == SHIFT_RIGHT)
	{
		// save the wrapped bytes
		
		for(i = block->width - ticks, j = 0; i < block->width; i++, j++)
		{
			tempRow[j] = getByte(block, i, row);
		}
		
		// copy body
		
		for(i = block->width - 1; i >= ticks; i--)
		{
			byte = getByte(block, i - ticks, row);
			setByte(byte, block, i, row);
		}
		
		// copy wrapped bytes
		
		for(i = 0; i < ticks; i++)
		{
			byte = tempRow[i];
			setByte(byte, block, i, row);
		}
	}
	
	for(unsigned int i = 0; i < block->width * block->height; i ++)
	{
		checksum2 += block->data[i];
	}
	
	if(checksum1 != checksum2)
	{
		printf("shifting row error: row %d\n", row);
	}
	
	return 1;
}

Byte getByte(struct DataBlock *block, int col, int row)
{
	return(block->data[col + row*block->width]);
}

Byte setByte(Byte value, struct DataBlock *block, int col, int row)
{
	block->data[col + row*block->width] = value;
	return value;
}

void printBlocks(struct DataBlock *first)
{
	char *string = malloc(first->width + 1);
	int numBlocks = 0;
	
	string[first->width] = '\0';
	
	while(1)
	{
		printf("Block %d", numBlocks);
	
		for(unsigned int i = 0; i < first->height; i++)
		{
			for(unsigned int ii = 0; ii < first->width; ii++)
			{
				string[i] = (char) getByte(first, ii, i);
			}

			printf("%s\n", string + 1);
		}

		if(first->next == NULL)
		{
			return;
		}
		else
		{
			first = first->next;
		}

		numBlocks++;
	}
}

/* The actual encryption stuff, putting all the code together :) */

void scrambleBlockData(struct DataBlock *first, char *key)
{
	printf("Scrambling data\n");
	
	unsigned int blockNum = 1;
	unsigned int keyLen, keySum;
	unsigned int col, row, colTicks, rowTicks;
	enum Direction colDirection, rowDirection;
	unsigned int loopLen, loopTicks;
	unsigned int a, b, c, d, e, f, g;
	
	const unsigned int seedsLen = 256;
	unsigned int seeds1[256] = {2086415110, 3596784488, 2052297812, 1132699022, 2530478996, 470460936, 4074303221, 3560729855, 2816606610, 3677724586, 1266663920, 2431711568, 4111360060, 3889468708, 2482733696, 3506094432, 588465066, 2915559976, 3635200752, 749775638, 566916508, 2451245392, 3304193936, 2136715776, 3340588496, 324708224, 4031292500, 3477555012, 1204470240, 3097635164, 1053731248, 9116784, 2352203522, 4038947556, 2091368767, 1368223806, 3490934056, 3026164720, 1341366880, 3350288904, 783176054, 48491410, 1830819598, 1542028125, 3913110916, 1517262964, 2913010240, 171437619, 3863174192, 1014507464, 3109120127, 446131020, 2625930144, 2823309809, 844783616, 3153831942, 1806793064, 3891302096, 2618716800, 2700698144, 574721804, 2589499513, 3565328472, 4270242800, 3637454072, 2143383992, 4204493038, 1073820270, 1691188736, 4127994160, 2411977392, 3287321472, 195098752, 2016888036, 835968500, 3858623280, 4195517716, 3567255582, 1072487908, 2511918212, 203714960, 2157781418, 4165314048, 2660025984, 2468112896, 2494376066, 1877148544, 2390139290, 2680694192, 1597891696, 2492166191, 998758528, 1238731897, 2925172830, 3993275800, 956992268, 2388525648, 332132265, 132579088, 2563066484, 2242264512, 3680042540, 2529520160, 176938296, 4147498586, 1025112448, 3074995720, 1355579208, 1625368288, 905531404, 2047995184, 2379787470, 546820495, 1577903752, 2592130840, 1393406120, 4103390758, 1802354348, 939961944, 3419826516, 652372217, 1337074672, 2680893620, 3295248547, 2172221074, 2056316256, 2655213568, 1951300218, 1052017520, 3545415730, 3882420224, 2966088576, 1896284261, 3404405410, 3042534556, 1306145214, 4078519216, 2861013244, 2590183468, 3660567688, 2445978272, 1585496100, 1934610124, 599107456, 705641156, 100121424, 3433210262, 3684843578, 1313444476, 170621140, 3088983131, 1787810311, 4261091064, 4024099836, 3206594383, 2034407104, 200952056, 3751633920, 3125442083, 2044208444, 2098748722, 605408864, 1983640984, 4022452448, 1398011584, 2298421056, 3586927843, 4234589932, 1687153226, 3505910672, 4012974656, 2098594874, 3576675232, 2248950822, 3907363348, 130500288, 483459835, 653552970, 3748483816, 1185529984, 2720522830, 2479680056, 2802407398, 3203996236, 2656636736, 686294548, 3469359374, 4251289616, 3853623504, 1144207984, 3657198046, 2890170240, 3231581082, 2274092956, 3582746232, 2369724640, 105297358, 2414765224, 1665021768, 921889772, 3000365036, 2532102469, 3490673962, 1170829068, 900251904, 4248710544, 996739784, 1933828652, 81800152, 899825858, 926269088, 2907392396, 4073871636, 4014062416, 1258152160, 215259592, 71878176, 3376217526, 1296525395, 3206270048, 557613616, 1587047650, 1648069168, 1304755052, 3445439144, 2953208294, 2121956192, 3290742607, 1101786428, 2306024060, 1265989064, 3858737234, 762844392, 3728396191, 1749389308, 1307776440, 2869450300, 1342624024, 1641635728, 1566538048, 2079512272, 1141694196, 577433120, 2237970772, 3074384952, 838555028, 3241693428, 3327673052, 598159710, 3962291819, 247432008, 3688870144, 2187076434, 1342179970, 3731689372, 116611665};
	unsigned int seeds2[256] = {644993504, 4078192842, 3088969856, 2326648928, 3809563648, 3842401984, 3005999104, 828873056, 2072143872, 192109952, 4024966528, 974618000, 3456152992, 2876529760, 3441437366, 633778990, 2743997824, 4208394784, 2713469536, 1687577920, 3117746096, 1760810688, 1044227008, 807748089, 1357618272, 3650067360, 2447706262, 1115922784, 3017564656, 3193711104, 2716798829, 3403281408, 2312845592, 17103624, 341650576, 3718266572, 979848008, 3113104072, 2490461120, 960452224, 2698649990, 1877566336, 2877903834, 3765144252, 4214195040, 2321043292, 794012923, 1746087968, 1162078152, 994745952, 2584554368, 916989152, 281948160, 2180084160, 3964778240, 281135072, 4040327976, 2300038784, 987361798, 3853676860, 821353432, 3161801984, 2982877712, 2091739832, 2568296704, 879364650, 2295564456, 3735620632, 2394074880, 1315302720, 399809720, 1362706416, 3259236352, 458870144, 271683200, 1702262412, 407445504, 3900198624, 2043636224, 1313118328, 4132065280, 3428542312, 3512402928, 1031230432, 4166345984, 3897320768, 127492256, 1480717912, 1434146176, 3042814067, 2502378272, 712026450, 3363278368, 1016241812, 3904864704, 776522656, 1802749504, 713075760, 1890456384, 1107939968, 62090912, 910183136, 754418560, 3194689128, 4113721968, 3648413842, 2544700172, 2937488890, 1812444016, 95726672, 3998897408, 2174015144, 3598248084, 4205027332, 4256940544, 2222437056, 2728449664, 699412008, 1882432424, 2877760604, 2590189328, 1147658556, 4008710912, 1086573056, 345409504, 1493929312, 580634372, 3117923462, 1847297528, 615739604, 2319051661, 476363776, 3219326260, 1858126912, 420119304, 666229984, 2801585276, 2735334464, 3089557760, 114405472, 105413608, 1045635072, 1464156516, 3398248252, 3332639040, 1420675556, 3852045440, 2500988478, 2313118144, 644045682, 1704780360, 4153270272, 3493485346, 31691328, 341214528, 249285376, 2508854576, 641875628, 2558812544, 2073123092, 467173376, 1016886500, 2270957924, 3426235760, 497963392, 3250908164, 703125760, 3774561408, 1093913792, 2679347920, 2453097674, 2178882080, 1937654784, 3240157992, 3748479570, 3930526964, 3615287808, 1400637712, 95070208, 2360734068, 3150007600, 3992498240, 3481475616, 3745843752, 873131008, 3325937849, 2666280944, 1364438016, 772064822, 3557904300, 1205154412, 3260701824, 3214554112, 2864208912, 1110467008, 3343190468, 2635013632, 1850323128, 4191225626, 685166656, 2870893568, 194104376, 3505652832, 743824212, 1767710632, 2679781076, 1579063296, 1795401306, 4240078760, 3608098184, 1449125216, 3793140468, 2375420368, 373553360, 163264796, 2172690615, 3307391760, 1712575792, 790451000, 2582764478, 1798966928, 3892357632, 205008896, 2294770304, 232912576, 1408350752, 3891511446, 3781792806, 4289013832, 3066702096, 752391928, 719336368, 3523456208, 1428396928, 413389643, 2720301184, 2083402296, 1026290944, 2059149536, 282547456, 2370846720, 1332413840, 2057230832, 1723941696, 2781053664, 2310529824, 1874804120, 1586434601, 2710034744, 167954088, 2064537472, 3777703166, 1285248256, 3466018200, 489477312, 699257312};
	unsigned int seeds3[256] = {2451904178, 3768321724, 1787017728, 1481704220, 485430272, 4198805504, 4192449368, 3912695296, 235001384, 1731428352, 3408591104, 796630304, 2661226496, 3716728072, 2222596224, 3546781696, 2534393332, 814775928, 2073598976, 4036165632, 3342105472, 4049580032, 1140796800, 2002684992, 3021500294, 1269199744, 231307842, 3774017536, 3636246528, 4158914560, 226492928, 4080570368, 3640328704, 3649283776, 356221792, 3512242112, 560292480, 1971330752, 2738400544, 3326544000, 3872021720, 875607496, 1931702272, 2583474560, 2881991280, 629540320, 1943969792, 1758627296, 3360762368, 2659457024, 3684747613, 2690011912, 409105024, 19759104, 1036304320, 4187287552, 3887419422, 3776612160, 2978975744, 1747733760, 3607216128, 3813910304, 990495184, 2967338368, 2710137088, 1120630144, 1844262400, 2128447104, 1104404480, 405410960, 2073258144, 189539328, 3724910656, 675203616, 906269904, 1956458336, 944458032, 1975133264, 4085309376, 1520312016, 3666466240, 83847808, 1867590656, 1694690160, 1985626880, 2488578720, 2195160368, 3296717152, 731485696, 2319309312, 3094741760, 1798593280, 1376895790, 2701061888, 409600320, 3265511296, 4198192768, 3093815296, 855433216, 2762475264, 3605498381, 904411024, 3133053632, 2250930468, 2550518784, 3861419008, 3524523472, 2686379712, 1317124065, 3696492544, 4265673904, 2505089024, 131399680, 2389249540, 1071121920, 1669684204, 1498420688, 4086656616, 176442688, 2653661048, 760345600, 305825536, 4167009232, 82949888, 1882101688, 1559078912, 3257247872, 2649557760, 527819488, 3784785408, 3934647808, 3124843080, 381199360, 3580366580, 3876849472, 2611953664, 580968448, 1531486400, 2720812800, 4117024832, 1140950272, 542708736, 665124864, 3893749324, 1130870320, 987943240, 164058416, 467916800, 1036238728, 3606445391, 4032820480, 305397408, 158666340, 4028061984, 3814548544, 1920702720, 2617111520, 245029888, 3289328814, 332748672, 4020133956, 2978587136, 3947877664, 2324829008, 4271091360, 951039520, 3107560800, 2233032704, 825202056, 70688304, 1163975808, 3311125440, 3177469952, 82428160, 703599072, 3354388992, 816371200, 262541760, 2768227760, 4261351456, 3872928256, 2383585568, 746710532, 191931200, 4284215488, 708627456, 1075384960, 2598584320, 3774395932, 763603660, 1703468032, 24782880, 3594161408, 1334307776, 2496831488, 1890524672, 632369152, 2610674624, 2978160640, 3421945816, 248876080, 263494288, 585184064, 2351081488, 1685079560, 3522519712, 695917056, 3071464016, 2556602928, 284977664, 1673035776, 583263904, 1161718784, 986742784, 3197206528, 857483520, 3575374720, 3767223020, 3754878816, 1304434688, 1000304096, 1982849536, 203644492, 2666216480, 1711722112, 941377664, 34416896, 1132346112, 366658816, 127070208, 783542528, 287818656, 4026261920, 2454833472, 397363880, 335477248, 3634166224, 3054633760, 4124571648, 433902592, 3993568768, 829055744, 1772416768, 1631254784, 751036064, 3120922144, 2997756928, 430242688, 2180009208, 3904491840, 1675857576, 2188115968, 865979328, 3566838544, 1314853744, 2534523136};
	
	keyLen = strlen(key);
	keySum = 0;
	for(int i = 0; key[i] != '\0'; i++)
	{
		keySum += key[i];
	}
	
	while(1)
	{
		loopLen = (keySum * keyLen * seeds1[keySum % seedsLen]) % 50 + 20;
		loopTicks = 1;
		printf("loopLen:%u", loopLen);
		
		while(loopTicks <= loopLen)
		{
			printf("loop %u:\n", loopTicks);
		
			a = key[loopTicks % keyLen] / keyLen * keySum * (loopTicks % (seeds1[loopTicks % seedsLen]));
			b = loopTicks * keySum | seeds2[a % seedsLen];
			c = key[loopLen % keyLen] * seeds1[keyLen % seedsLen] * seeds2[keySum % seedsLen] % seeds3[b % seedsLen];
			d = (loopLen % loopTicks) * c ^ a + b;
			e = key[loopTicks % keyLen] * ~(seeds3[(seeds2[(seeds1[(a * b * c * d) % seedsLen]) % seedsLen]) % seedsLen]);
			f = loopTicks * loopLen + (d % a * e);
			g = key[loopLen % keyLen] * seeds1[loopTicks % seedsLen] * (seeds2[(f ^ e) % seedsLen] & d) + seeds3[(~loopTicks) % seedsLen];
			
			col = (a * b) % first->width;
			row = (b * c) % first->height;
			
			colTicks = (c % d) % first->height;
			rowTicks = (d * e * f) % first->width;
			
			colDirection = ((a ^ b) & (c | d));
			colDirection &= 1;
			rowDirection = (((d * e) ^ g) | (g * a));
			rowDirection &= 1;
			
			if(colDirection == 0)
			{
				colDirection = SHIFT_UP;
			}
			else
			{
				colDirection = SHIFT_DOWN;
			}
			
			if(rowDirection == 0)
			{
				rowDirection = SHIFT_LEFT;
			}
			else
			{
				rowDirection = SHIFT_RIGHT;
			}
			
			printf("action:: col:%u, row: %u, colTicks:%u, rowTicks:%u, colDir:%u, rowDir:%u\n", col, row, colTicks, rowTicks, colDirection, rowDirection);
			
			if(!(shiftCol(first, col, colTicks, colDirection)))
			{
				printf("shifting data column failure\n");
				return;
			}
			if(!(shiftRow(first, row, rowTicks, rowDirection)))
			{
				printf("shifting data row failure\n");
				return;
			}
			
			loopTicks++;
		}
		
		blockNum++;
		if(first->next != NULL)
		{
			first = first->next;
		}
		else
		{
			return;
		}
	}
	
	return;
}

void unscrambleBlockData(struct DataBlock *first, char *key)
{
	printf("Unscrambling data\n");
	
	unsigned int blockNum, numBlocks;
	struct DataBlock *block, **blockList;
	
	unsigned int keyLen, keySum;
	
	unsigned int col, row, colTicks, rowTicks;
	enum Direction colDirection, rowDirection;
	
	unsigned int loopLen, loopTicks;
	unsigned int a, b, c, d, e, f, g;
	
	const unsigned int seedsLen = 256;
	unsigned int seeds1[256] = {2086415110, 3596784488, 2052297812, 1132699022, 2530478996, 470460936, 4074303221, 3560729855, 2816606610, 3677724586, 1266663920, 2431711568, 4111360060, 3889468708, 2482733696, 3506094432, 588465066, 2915559976, 3635200752, 749775638, 566916508, 2451245392, 3304193936, 2136715776, 3340588496, 324708224, 4031292500, 3477555012, 1204470240, 3097635164, 1053731248, 9116784, 2352203522, 4038947556, 2091368767, 1368223806, 3490934056, 3026164720, 1341366880, 3350288904, 783176054, 48491410, 1830819598, 1542028125, 3913110916, 1517262964, 2913010240, 171437619, 3863174192, 1014507464, 3109120127, 446131020, 2625930144, 2823309809, 844783616, 3153831942, 1806793064, 3891302096, 2618716800, 2700698144, 574721804, 2589499513, 3565328472, 4270242800, 3637454072, 2143383992, 4204493038, 1073820270, 1691188736, 4127994160, 2411977392, 3287321472, 195098752, 2016888036, 835968500, 3858623280, 4195517716, 3567255582, 1072487908, 2511918212, 203714960, 2157781418, 4165314048, 2660025984, 2468112896, 2494376066, 1877148544, 2390139290, 2680694192, 1597891696, 2492166191, 998758528, 1238731897, 2925172830, 3993275800, 956992268, 2388525648, 332132265, 132579088, 2563066484, 2242264512, 3680042540, 2529520160, 176938296, 4147498586, 1025112448, 3074995720, 1355579208, 1625368288, 905531404, 2047995184, 2379787470, 546820495, 1577903752, 2592130840, 1393406120, 4103390758, 1802354348, 939961944, 3419826516, 652372217, 1337074672, 2680893620, 3295248547, 2172221074, 2056316256, 2655213568, 1951300218, 1052017520, 3545415730, 3882420224, 2966088576, 1896284261, 3404405410, 3042534556, 1306145214, 4078519216, 2861013244, 2590183468, 3660567688, 2445978272, 1585496100, 1934610124, 599107456, 705641156, 100121424, 3433210262, 3684843578, 1313444476, 170621140, 3088983131, 1787810311, 4261091064, 4024099836, 3206594383, 2034407104, 200952056, 3751633920, 3125442083, 2044208444, 2098748722, 605408864, 1983640984, 4022452448, 1398011584, 2298421056, 3586927843, 4234589932, 1687153226, 3505910672, 4012974656, 2098594874, 3576675232, 2248950822, 3907363348, 130500288, 483459835, 653552970, 3748483816, 1185529984, 2720522830, 2479680056, 2802407398, 3203996236, 2656636736, 686294548, 3469359374, 4251289616, 3853623504, 1144207984, 3657198046, 2890170240, 3231581082, 2274092956, 3582746232, 2369724640, 105297358, 2414765224, 1665021768, 921889772, 3000365036, 2532102469, 3490673962, 1170829068, 900251904, 4248710544, 996739784, 1933828652, 81800152, 899825858, 926269088, 2907392396, 4073871636, 4014062416, 1258152160, 215259592, 71878176, 3376217526, 1296525395, 3206270048, 557613616, 1587047650, 1648069168, 1304755052, 3445439144, 2953208294, 2121956192, 3290742607, 1101786428, 2306024060, 1265989064, 3858737234, 762844392, 3728396191, 1749389308, 1307776440, 2869450300, 1342624024, 1641635728, 1566538048, 2079512272, 1141694196, 577433120, 2237970772, 3074384952, 838555028, 3241693428, 3327673052, 598159710, 3962291819, 247432008, 3688870144, 2187076434, 1342179970, 3731689372, 116611665};
	unsigned int seeds2[256] = {644993504, 4078192842, 3088969856, 2326648928, 3809563648, 3842401984, 3005999104, 828873056, 2072143872, 192109952, 4024966528, 974618000, 3456152992, 2876529760, 3441437366, 633778990, 2743997824, 4208394784, 2713469536, 1687577920, 3117746096, 1760810688, 1044227008, 807748089, 1357618272, 3650067360, 2447706262, 1115922784, 3017564656, 3193711104, 2716798829, 3403281408, 2312845592, 17103624, 341650576, 3718266572, 979848008, 3113104072, 2490461120, 960452224, 2698649990, 1877566336, 2877903834, 3765144252, 4214195040, 2321043292, 794012923, 1746087968, 1162078152, 994745952, 2584554368, 916989152, 281948160, 2180084160, 3964778240, 281135072, 4040327976, 2300038784, 987361798, 3853676860, 821353432, 3161801984, 2982877712, 2091739832, 2568296704, 879364650, 2295564456, 3735620632, 2394074880, 1315302720, 399809720, 1362706416, 3259236352, 458870144, 271683200, 1702262412, 407445504, 3900198624, 2043636224, 1313118328, 4132065280, 3428542312, 3512402928, 1031230432, 4166345984, 3897320768, 127492256, 1480717912, 1434146176, 3042814067, 2502378272, 712026450, 3363278368, 1016241812, 3904864704, 776522656, 1802749504, 713075760, 1890456384, 1107939968, 62090912, 910183136, 754418560, 3194689128, 4113721968, 3648413842, 2544700172, 2937488890, 1812444016, 95726672, 3998897408, 2174015144, 3598248084, 4205027332, 4256940544, 2222437056, 2728449664, 699412008, 1882432424, 2877760604, 2590189328, 1147658556, 4008710912, 1086573056, 345409504, 1493929312, 580634372, 3117923462, 1847297528, 615739604, 2319051661, 476363776, 3219326260, 1858126912, 420119304, 666229984, 2801585276, 2735334464, 3089557760, 114405472, 105413608, 1045635072, 1464156516, 3398248252, 3332639040, 1420675556, 3852045440, 2500988478, 2313118144, 644045682, 1704780360, 4153270272, 3493485346, 31691328, 341214528, 249285376, 2508854576, 641875628, 2558812544, 2073123092, 467173376, 1016886500, 2270957924, 3426235760, 497963392, 3250908164, 703125760, 3774561408, 1093913792, 2679347920, 2453097674, 2178882080, 1937654784, 3240157992, 3748479570, 3930526964, 3615287808, 1400637712, 95070208, 2360734068, 3150007600, 3992498240, 3481475616, 3745843752, 873131008, 3325937849, 2666280944, 1364438016, 772064822, 3557904300, 1205154412, 3260701824, 3214554112, 2864208912, 1110467008, 3343190468, 2635013632, 1850323128, 4191225626, 685166656, 2870893568, 194104376, 3505652832, 743824212, 1767710632, 2679781076, 1579063296, 1795401306, 4240078760, 3608098184, 1449125216, 3793140468, 2375420368, 373553360, 163264796, 2172690615, 3307391760, 1712575792, 790451000, 2582764478, 1798966928, 3892357632, 205008896, 2294770304, 232912576, 1408350752, 3891511446, 3781792806, 4289013832, 3066702096, 752391928, 719336368, 3523456208, 1428396928, 413389643, 2720301184, 2083402296, 1026290944, 2059149536, 282547456, 2370846720, 1332413840, 2057230832, 1723941696, 2781053664, 2310529824, 1874804120, 1586434601, 2710034744, 167954088, 2064537472, 3777703166, 1285248256, 3466018200, 489477312, 699257312};
	unsigned int seeds3[256] = {2451904178, 3768321724, 1787017728, 1481704220, 485430272, 4198805504, 4192449368, 3912695296, 235001384, 1731428352, 3408591104, 796630304, 2661226496, 3716728072, 2222596224, 3546781696, 2534393332, 814775928, 2073598976, 4036165632, 3342105472, 4049580032, 1140796800, 2002684992, 3021500294, 1269199744, 231307842, 3774017536, 3636246528, 4158914560, 226492928, 4080570368, 3640328704, 3649283776, 356221792, 3512242112, 560292480, 1971330752, 2738400544, 3326544000, 3872021720, 875607496, 1931702272, 2583474560, 2881991280, 629540320, 1943969792, 1758627296, 3360762368, 2659457024, 3684747613, 2690011912, 409105024, 19759104, 1036304320, 4187287552, 3887419422, 3776612160, 2978975744, 1747733760, 3607216128, 3813910304, 990495184, 2967338368, 2710137088, 1120630144, 1844262400, 2128447104, 1104404480, 405410960, 2073258144, 189539328, 3724910656, 675203616, 906269904, 1956458336, 944458032, 1975133264, 4085309376, 1520312016, 3666466240, 83847808, 1867590656, 1694690160, 1985626880, 2488578720, 2195160368, 3296717152, 731485696, 2319309312, 3094741760, 1798593280, 1376895790, 2701061888, 409600320, 3265511296, 4198192768, 3093815296, 855433216, 2762475264, 3605498381, 904411024, 3133053632, 2250930468, 2550518784, 3861419008, 3524523472, 2686379712, 1317124065, 3696492544, 4265673904, 2505089024, 131399680, 2389249540, 1071121920, 1669684204, 1498420688, 4086656616, 176442688, 2653661048, 760345600, 305825536, 4167009232, 82949888, 1882101688, 1559078912, 3257247872, 2649557760, 527819488, 3784785408, 3934647808, 3124843080, 381199360, 3580366580, 3876849472, 2611953664, 580968448, 1531486400, 2720812800, 4117024832, 1140950272, 542708736, 665124864, 3893749324, 1130870320, 987943240, 164058416, 467916800, 1036238728, 3606445391, 4032820480, 305397408, 158666340, 4028061984, 3814548544, 1920702720, 2617111520, 245029888, 3289328814, 332748672, 4020133956, 2978587136, 3947877664, 2324829008, 4271091360, 951039520, 3107560800, 2233032704, 825202056, 70688304, 1163975808, 3311125440, 3177469952, 82428160, 703599072, 3354388992, 816371200, 262541760, 2768227760, 4261351456, 3872928256, 2383585568, 746710532, 191931200, 4284215488, 708627456, 1075384960, 2598584320, 3774395932, 763603660, 1703468032, 24782880, 3594161408, 1334307776, 2496831488, 1890524672, 632369152, 2610674624, 2978160640, 3421945816, 248876080, 263494288, 585184064, 2351081488, 1685079560, 3522519712, 695917056, 3071464016, 2556602928, 284977664, 1673035776, 583263904, 1161718784, 986742784, 3197206528, 857483520, 3575374720, 3767223020, 3754878816, 1304434688, 1000304096, 1982849536, 203644492, 2666216480, 1711722112, 941377664, 34416896, 1132346112, 366658816, 127070208, 783542528, 287818656, 4026261920, 2454833472, 397363880, 335477248, 3634166224, 3054633760, 4124571648, 433902592, 3993568768, 829055744, 1772416768, 1631254784, 751036064, 3120922144, 2997756928, 430242688, 2180009208, 3904491840, 1675857576, 2188115968, 865979328, 3566838544, 1314853744, 2534523136};
	
	keyLen = strlen(key);
	keySum = 0;
	for(int i = 0; key[i] != '\0'; i++)
	{
		keySum += key[i];
	}
	
	// store a list of all the blocks so that I can work backward
	
	numBlocks = getNumBlocks(first);
	blockList = malloc(sizeof(struct DataBlock) * numBlocks);
	
	block = first;
	for(unsigned int i = 0; i < numBlocks; i++)
	{
		blockList[i] = block;
		block = block->next;
	}
	
	for(blockNum = numBlocks; blockNum > 0; blockNum--)
	{
		block = blockList[blockNum - 1];
		
		loopLen = (keySum * keyLen * seeds1[keySum % seedsLen]) % 50 + 20;
		loopTicks = loopLen;
		printf("LoopLen:%d\n", loopLen);
		
		while(loopTicks > 0)
		{
			printf("loop %u:\n", loopTicks);
			a = key[loopTicks % keyLen] / keyLen * keySum * (loopTicks % (seeds1[loopTicks % seedsLen]));
			b = loopTicks * keySum | seeds2[a % seedsLen];
			c = key[loopLen % keyLen] * seeds1[keyLen % seedsLen] * seeds2[keySum % seedsLen] % seeds3[b % seedsLen];
			d = (loopLen % loopTicks) * c ^ a + b;
			e = key[loopTicks % keyLen] * ~(seeds3[(seeds2[(seeds1[(a * b * c * d) % seedsLen]) % seedsLen]) % seedsLen]);
			f = loopTicks * loopLen + (d % a * e);
			g = key[loopLen % keyLen] * seeds1[loopTicks % seedsLen] * (seeds2[(f ^ e) % seedsLen] & d) + seeds3[(~loopTicks) % seedsLen];
			
			col = (a * b) % block->width;
			row = (b * c) % block->height;
			
			colTicks = (c % d) % block->height;
			rowTicks = (d * e * f) % block->width;
			
			colDirection = ((a ^ b) & (c | d));
			colDirection &= 1;
			rowDirection = (((d * e) ^ g) | (g * a));
			rowDirection &= 1;
			
			if(colDirection == 0)
			{
				colDirection = SHIFT_DOWN;
			}
			else
			{
				colDirection = SHIFT_UP;
			}
			
			if(rowDirection == 0)
			{
				rowDirection = SHIFT_RIGHT;
			}
			else
			{
				rowDirection = SHIFT_LEFT;
			}
			
			printf("action:: col:%u, row: %u, colTicks:%u, rowTicks:%u, colDir:%u, rowDir:%u\n", col, row, colTicks, rowTicks, colDirection, rowDirection);
			
			// make sure the shifting works
			
			if(!(shiftRow(block, row, rowTicks, rowDirection)))
			{
				printf("shifting data row failure\n");
				return;
			}
			if(!(shiftCol(block, col, colTicks, colDirection)))
			{
				printf("shifting data column failure\n");
				return;
			}
			
			loopTicks--;
		}
	}
	
	return;
}

unsigned int getNumBlocks(struct DataBlock *first)
{
	unsigned int num = 1;
	while(first->next != NULL)
	{
		first = first->next;
		num++;
	}
	return num;
}