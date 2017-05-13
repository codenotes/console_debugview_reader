
#include "stdafx.h"

#ifdef _WIN32

#define KEY_UP 72
#define KEY_DOWN 80
#define KEY_LEFT 75
#define KEY_RIGHT 77


#endif

ANSI_Util au;
#define INPUT_LINE 10
#define INPUT_COL 10

#define LOG(...) ANSI_Util::OutputDebugStringV(__VA_ARGS__)


void testtabcb(KEY_TYPE k, KEY_TYPE modifier)
{
	using namespace std;

	unsigned char ch;
	bool vk;
	static int loc = INPUT_COL;
	static std::vector<unsigned char> line;
	static int prev_line_size = 0;

	auto delAndReprint = [&](int iline, int icol)
	{
		printf(CURSOR_SAVE);
		//line.insert(line.begin() + (loc - input_col), ch);

		ANSI_Util::CurPos(iline, icol);
		printf(ECH, prev_line_size); //clear previous

		for (auto c : line) //print the whole line from scratch
		{
			printf("%c", c);
		}

		printf(CURSOR_RESTORE);
	};

	prev_line_size = line.size();

	au.CurPos(INPUT_LINE, loc);

	auto it = line.begin();

	if (au.translateChar(k, modifier, ch, vk))
	{
		if (!vk)
		{

			if (loc - INPUT_COL == line.size())//is cursor at end? If so, append
			{
				line.push_back(ch);
				printf("%c", ch);// , loc++;
			}
			else //cursor inside somewhere
			{



				line.insert(line.begin() + (loc - INPUT_COL), ch);

				//	deleteAndReprint(loc, prev_line_size, INPUT_LINE, INPUT_COL, line);
				delAndReprint(INPUT_LINE, INPUT_COL);


			}

			loc++;
		}
		else//to do, switch case depending on what, like RET
		{
			switch (k)
			{
			case VK_SPACE:

				if (!(loc - INPUT_COL == line.size()))//we are inside somewhere
				{

					line.insert(line.begin() + (loc - INPUT_COL), ' ');
					delAndReprint(INPUT_LINE, INPUT_COL);
					loc++;
				}
				else //at end, normal
				{
					printf(" ", loc += 1);
					line.push_back(' ');
				}
				break;

			case VK_RETURN:
				au.CurPos(INPUT_LINE, INPUT_COL);
				printf(ECH, loc + 1);
				loc = 10;
				//printf("[ret]"),loc+=5;
				break;

			case VK_BACK:
				//am I inside the string?
				if (loc - INPUT_COL == 0)//at beginning, stop
				{
					break;
				}

				if (!(loc - INPUT_COL == line.size()))//we are inside somewhere
				{
					line.erase(line.begin() + (loc - INPUT_COL - 1));
					delAndReprint(INPUT_LINE, INPUT_COL);
					au.CurPos(INPUT_LINE, --loc);
				}
				else //back
				{
					au.CurPos(INPUT_LINE, --loc);
					printf(ECH, 1);
					line.pop_back();
					break;
				}
			case VK_DELETE:
				//	line.erase(line.begin() + loc-INPUT_COL);
				//	delAndReprint(INPUT_LINE , INPUT_COL);
				//printf("%c",7);//printf("del");
				//printf("\033[3~");

				break;

			case VK_LEFT:
				if (modifier & CTRL_KEY_MASK)
				{
					//find the last space in line from where we already are
					au.OutputDebugStringV("loc:%d loc-ic:%d", loc, INPUT_COL);

					auto reloc = loc - INPUT_COL;

					if (reloc == 0)
						break;

					vector<unsigned char>::reverse_iterator it;



					if (line[reloc - 1] == ' ') //peek left
					{

						//		au.OutputDebugStringV("***reloc:%d size-reloc:%d", reloc, line.size()- reloc);
						it = line.rbegin() + (line.size() - reloc);
						//	au.OutputDebugStringV("1we are to the right of a space. should  jump over it, char:>%c<", *it);

						if (it == line.rend())
						{
							break;
						}


						it += 2;
						//	au.OutputDebugStringV("2we are to the right of a space. should  jump over it, char:>%c<", *it);
						//	it++;

						it = find(it, line.rend(), ' ');
						if (it == line.rend())
							;//	break;
							 //	au.OutputDebugStringV("3we are to the right of a space. should  jump over it, char:>%c<", *it);
							 /*int i;
							 for (i = reloc - 2; line[i] != ' ' && i >= 0; i--)
							 {
							 au.OutputDebugStringV("found ' ' at %d, reloc is:%d", i,reloc);
							 it = line.rbegin() + (reloc-i +1);
							 }
							 */

							 //std::advance(it, 1);
							 /*	it = line.rbegin();
							 au.OutputDebugStringV("we are to the right of a space. should  jump over it, char:%c<", *(line.rbegin()));
							 it += 1;
							 it = find(it, line.rend(), ' ');*/
					}
					else
					{
						it = find(line.rbegin(), line.rend(), ' ');

					}

					//+ (loc - INPUT_COL)
					if (it != line.rend())
					{
						loc = it - line.rbegin(); //how many chars back from end of string (+1 for the cursor which is beyond it)
												  //loc=std::distance(line.rbegin(), it);
						loc = line.size() - loc + INPUT_COL;

						au.CurPos(INPUT_LINE, loc);

					}
					else
					{
						au.CurPos(INPUT_LINE, loc = INPUT_COL);//go to home position
					}

				}
				else
				{
					au.CurPos(INPUT_LINE, --loc);

				}
				break;

			case VK_RIGHT:
				if (loc - INPUT_COL == line.size())//we are at end
				{


					au.OutputDebugStringV("NO!!");
					break;
				}
				//au.CurPos(INPUT_LINE, ++loc);
				if (modifier & CTRL_KEY_MASK)
				{
					auto reloc = loc - INPUT_COL;
					//find the last space in line

					vector<unsigned char>::iterator it;

					au.OutputDebugStringV("*1-loc:%d reloc:%d", loc, reloc);


					au.OutputDebugStringV("*3-loc:%d reloc:%d", loc, reloc);
					it = line.begin() + reloc;
					au.OutputDebugStringV("*4-loc:%d reloc:%d", loc, reloc);
					if (reloc >= line.size())
					{
						break;
					}



					it = find(it, line.end(), ' ');

					if (it == line.end())
					{
						loc = line.size() + INPUT_COL; //might as well rest at the very end of the line
						au.CurPos(INPUT_LINE, loc);
						au.OutputDebugStringV("BREAKING 1");
						break;
					}
					else
						it += 2;

					au.OutputDebugStringV("5-at loc:%d reloc:%d", loc, reloc);

					if (it != line.end())
					{

						loc += std::distance(line.begin() + reloc, it) - 1;
						au.OutputDebugStringV("6-would advance cursor to:%d", loc);
						au.CurPos(INPUT_LINE, loc);

					}
					else
					{
						au.CurPos(INPUT_LINE, loc = INPUT_COL + line.size());//go to END position
					}

				}
				else
				{
					au.OutputDebugStringV("surprise");
					au.CurPos(INPUT_LINE, ++loc);

				}


				break;

			case VK_END:
				au.CurPos(INPUT_LINE, loc = INPUT_COL + line.size());
				break;

			case VK_HOME:
				au.CurPos(INPUT_LINE, loc = INPUT_COL);
				break;



			}
		}

	}

	delAndReprint(INPUT_LINE + 1, INPUT_COL);
	//printf(CURSOR_SAVE);
	//au.CurPos(INPUT_LINE+1, INPUT_COL);
	//printf(ECH, prev_line_size);// line.size());

	//for (auto c : line)
	//{
	//	printf("%c",c);
	//}
	//printf(CURSOR_RESTORE);




}

void testANSI()
{
#ifdef _WIN32


	au.EnableVTMode();
	au.AddLoc("one", 13, 13);
	au.AddLoc("input", 20, 20);
	au.AddLoc("within_scroll", 2, 1);
	au.StoreScrollingRegionLocation("scroll1", 1, 5);
	au.SetScrollingRegion("scroll1");

	au.StoreScrollingRegionLocation("scroll2", 10, 3);
	au.SetScrollingRegion("scroll2");

	int i = 0;


	au.AppendScrollingRegion("scroll1", ANSI_Util::BOLDGREEN, "hi");
	au.AppendScrollingRegion("scroll1", ANSI_Util::BOLDGREEN, "there");
	au.AppendScrollingRegion("scroll1", ANSI_Util::BOLDGREEN, "greg");
	//au.AppendScrollingRegion("scroll1", ANSI_Util::BOLDGREEN, "brill");
	//au.AppendScrollingRegion("scroll1", ANSI_Util::BOLDGREEN, "handsome");

	au.AppendScrollingRegion("scroll2", ANSI_Util::BOLDGREEN, "one");
	au.AppendScrollingRegion("scroll2", ANSI_Util::BOLDGREEN, "two");
	au.AppendScrollingRegion("scroll2", ANSI_Util::BOLDGREEN, "three");
	au.AppendScrollingRegion("scroll2", ANSI_Util::BOLDGREEN, "four");


	au.FillScrollingRegion("scroll1");
	au.FillScrollingRegion("scroll2");


	bool kup_pressed = false;


	//typedef boost::function<void(unsigned short)> testdef;
	//testdef td=	boost::bind(testANSI);

	auto b = boost::bind(testtabcb, _1, _2);
	au.AddKeyCallback('A', b);
	//au.AddKeyCallback(VK_TAB, b);


	char temp[255];
	//scanf("%s", temp);

	while (1)
	{


		switch (au.GetPressedKey(true))
		{
		case VK_UP:
			au.ScrollUp("scroll1");
			au.ScrollUp("scroll2");
			break;

		case VK_DOWN:
			au.ScrollDown("scroll1");
			au.ScrollDown("scroll2");
			break;



		}

		//if ((GetAsyncKeyState(VK_UP) & 0x1) == 0x1)
		//{
		//	au.ScrollUp("scroll1");
		//	au.ScrollUp("scroll2");
		//}

		//if ((GetAsyncKeyState(VK_DOWN) & 0x1) == 0x1)

		//{
		//	au.ScrollDown("scroll1");
		//	au.ScrollDown("scroll2");
		//}

		//
		//if ((GetAsyncKeyState(VK_RIGHT) & 0x1) == 0x1)
		//{
		//	printf(USE_ALT_BUFFER);
		//	printf("hi!");
		//}



		//if ((GetAsyncKeyState(VK_LEFT) & 0x1) == 0x1)
		//{
		//	printf(USE_MAIN_BUFFER);
		//}

		//if ((GetAsyncKeyState(VK_TAB) & 0x1) == 0x1)
		//{
		//	printf("tab");
		//}



	}

	au.ScrollDown("scroll1");
	au.ScrollDown("scroll1");
	au.ScrollDown("scroll1");
	au.ScrollDown("scroll1");

	au.ScrollUp("scroll1");
	au.ScrollUp("scroll1");

	au.CurPos(10, 10);
	return;
	printf(CURSOR_HIDE);
	while (i<1000)
	{

		printf("%d\n", i++);
		//	au.PrintAtLoc("one", ANSI_Util::colors::RED, "hi %d", i);

	}



	printf(CURSOR_SHOW);
	while (1)
	{

		au.GetInputSimpleAtLocation("input", temp, 25);

		au.PrintAtLoc("within_scroll", ANSI_Util::BOLDCYAN, "In scroll region\n");
		au.ScrollDown("scroll1", 1);
		//au.CurPos(1, 1);
		//	printf(SD,1);
		//au.CurPos(7, 1);
		//printf(temp);
		//printf("\n");


	}

	return;


	//au.StoreCurrentLocation("test");

#endif

}

