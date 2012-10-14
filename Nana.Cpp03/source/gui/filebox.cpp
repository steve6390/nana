#include <nana/gui/filebox.hpp>
#include <nana/filesystem/fs_utility.hpp>

#if defined(NANA_WINDOWS)
	#include <windows.h>
#elif defined(NANA_LINUX)
	#include <nana/gui/widgets/label.hpp>
	#include <nana/gui/widgets/button.hpp>
	#include <nana/gui/widgets/listbox.hpp>
	#include <nana/gui/widgets/categorize.hpp>
	#include <nana/gui/widgets/textbox.hpp>
	#include <nana/gui/widgets/treebox.hpp>
	#include <nana/gui/widgets/combox.hpp>
	#include <nana/filesystem/file_iterator.hpp>
	#include <nana/gui/layout.hpp>
	#include <nana/gui/functional.hpp>
	#include <stdexcept>
#endif

namespace nana{	namespace gui
{
#if defined(NANA_LINUX)
	class filebox_implement
		: public form
	{
		struct item_fs
		{
			nana::string name;
			::tm modified_time;
			bool directory;
			nana::long_long_t bytes;
		};

		class fs_resolver: public listbox::resolver_interface<item_fs>
		{
			static std::ostream& _m_stream2(std::stringstream& ss, unsigned v)
			{
				if(v < 10)	ss<<'0';
				return (ss<<v);
			}

			nana::string decode(std::size_t i, const item_fs& item) const
			{
				switch(i)
				{
				case 0:
					return item.name;
				case 1:
					{
						std::stringstream ss;
						ss<<(item.modified_time.tm_year + 1900)<<'-';
						_m_stream2(ss, item.modified_time.tm_mon)<<'-';
						_m_stream2(ss, item.modified_time.tm_mday)<<' ';

						_m_stream2(ss, item.modified_time.tm_hour)<<':';
						_m_stream2(ss, item.modified_time.tm_min)<<':';
						_m_stream2(ss, item.modified_time.tm_sec);
						return nana::charset(ss.str());
					}
				case 2:
					if(false == item.directory)
					{
						std::size_t pos = item.name.find_last_of(STR('.'));
						if(pos != item.name.npos && (pos + 1 < item.name.size()))
							return item.name.substr(pos + 1);
						return STR("File");
					}
					return STR("Directory");
				case 3:
					if(false == item.directory)
					{
						std::stringstream ss;
						if(item.bytes < 1024)
						{
							ss<<item.bytes<<" Bytes";
							return nana::charset(ss.str());
						}
						double size = item.bytes / 1024.0;
						if(size < 1024)
							return _m_trans(size) + STR(" KB");
						size /= 1024.0;
						if(size < 1024)
							return _m_trans(size) + STR(" MB");
						size /= 1024.0;
						if(size < 1024)
							return _m_trans(size) + STR(" GB");
						size /= 1024.0;
						return _m_trans(size) + STR(" TB");
					}
					return nana::string();
				}
				return nana::string();
			}

			void encode(item_fs& item, std::size_t i, const nana::string& s) const
			{
				switch(i)
				{
				case 0:
					item.name = s;
					break;
				case 2:
					item.directory = (s == STR("Directory"));
					break;
				}
			}

			nana::string _m_trans(double value) const
			{
				std::stringstream ss;
				ss<<value;
				std::string s = ss.str();
				std::size_t pos = s.find('.');
				if(pos != s.npos)
				{
					if(pos + 2 < s.size())
						s.erase(pos + 2);
				}
				return nana::charset(s);
			}
		};
		
		struct pred_sort_fs
		{
			bool operator()(const item_fs& a, const item_fs& b) const
			{
				return ((a.directory != b.directory) && a.directory);
			}
		};
	public:
		struct kind
		{
			enum t{none, filesystem};
		};

		typedef treebox<kind::t>::node_type node_type;
	public:

		filebox_implement(window owner, bool io_read, const nana::string& title)
			: form(owner, API::make_center(owner, 630, 440)), io_read_(io_read)
		{
			path_.create(*this);
			path_.splitstr(STR("/"));
			path_.ext_event().selected = nana::make_fun(*this, &filebox_implement::_m_cat_selected);

			filter_.create(*this);
			filter_.multi_lines(false);
			filter_.tip_string(STR("Filter"));
			filter_.make_event<events::key_up>(*this, &filebox_implement::_m_filter_changed);

			btn_folder_.create(*this);
			btn_folder_.caption(STR("&New Folder"));
			btn_folder_.make_event<events::click>(*this, &filebox_implement::_m_new_folder);

			tree_.create(*this);

			ls_file_.create(*this);
			ls_file_.append_header(STR("Name"), 190);
			ls_file_.append_header(STR("Modified"), 145);
			ls_file_.append_header(STR("Type"), 80);
			ls_file_.append_header(STR("Size"), 70);
			ls_file_.make_event<events::dbl_click>(*this, &filebox_implement::_m_sel_file);
			ls_file_.make_event<events::mouse_down>(*this, &filebox_implement::_m_sel_file);

			lb_file_.create(*this);
			lb_file_.caption(STR("File:"));
			
			tb_file_.create(*this);
			tb_file_.multi_lines(false);
			tb_file_.make_event<events::key_char>(*this, &filebox_implement::_m_keychar);

			cb_types_.create(*this);
			cb_types_.editable(false);
			cb_types_.ext_event().selected = nana::make_fun(*this, &filebox_implement::_m_cb_type_selected);

			btn_ok_.create(*this);
			btn_ok_.caption(STR("&OK"));
			btn_ok_.make_event<events::click>(*this, &filebox_implement::_m_ok);
			btn_cancel_.create(*this);
			btn_cancel_.caption(STR("&Cancel"));
			btn_cancel_.make_event<events::click>(destroy(*this));

			selection_.type = kind::none;
			_m_layout();
			_m_init_tree();
			_m_load_cat_path(nana::filesystem::path_user());

			if(0 == title.size())
				caption(io_read ? STR("Open") : STR("Save As"));
			else
				caption(title);
		}

		void def_extension(const nana::string& ext)
		{
			def_ext_ = ext;
		}

		void add_filter(const nana::string& desc, const nana::string& type)
		{
			std::size_t i = cb_types_.the_number_of_options();
			cb_types_.push_back(desc);
			if(0 == i)
				cb_types_.option(0);

			std::vector<nana::string> v;
			std::size_t beg = 0;
			while(true)
			{
				std::size_t pos = type.find(STR(';'), beg);
				nana::string ext = type.substr(beg, pos == type.npos ? type.npos : pos - beg);
				std::size_t dot = ext.find(STR('.'));
				if((dot != ext.npos) && (dot + 1 < ext.size()))
				{
					ext.erase(0, dot + 1);
					if(ext == STR("*"))
					{
						v.clear();
						break;
					}
					else
						v.push_back(ext);
				}
				if(pos == type.npos)
					break;
				beg = pos + 1;
			}
			if(v.size())
				cb_types_.anyobj(i, v);
		}

		bool selected() const
		{
			return (selection_.type != kind::none);
		}

		nana::string file() const
		{
			if(selection_.type != kind::none)
				return selection_.target;
			return nana::string();
		}
	private:
		void _m_layout()
		{
			gird_.bind(*this);
			gird * tmpgd = gird_.push(5, 24);
			tmpgd->add(path_, 5, 0);
			tmpgd->add(filter_, 5, 200);
			tmpgd->add(0, 5);
			
			tmpgd = gird_.push(5, 25);
			tmpgd->add(btn_folder_, 10, 90);

			tmpgd = gird_.push(5, 0);
			tmpgd->add(tree_, 0, 160);
			tmpgd->add(ls_file_,0, 0);

			tmpgd = gird_.push(12, 26);
			tmpgd->add(100, 60)->push(lb_file_, 5, 0);
			tmpgd->add(tb_file_, 5, 0);
			tmpgd->add(cb_types_, 10, 190);
			tmpgd->add(0, 18);

			tmpgd = gird_.push(8, 26);
			tmpgd->add(0, 0);
			tmpgd->add(btn_ok_, 0, 88);
			tmpgd->add(btn_cancel_, 14, 88);
			tmpgd->add(0, 18);

			gird_.push(0, 14);
			
		}

		void _m_init_tree()
		{
			nana::string home = nana::filesystem::path_user();

			//The path in linux is starting with the character '/', the name of root key should be
			//"FS.HOME", "FS.ROOT". Because a key of the tree widget must not be '/'

			nodes_.home = tree_.insert(STR("FS.HOME"), STR("Home"), kind::filesystem);
			nodes_.filesystem = tree_.insert(STR("FS.ROOT"), STR("Filesystem"), kind::filesystem);

			nana::filesystem::file_iterator end;
			nana::string str = nana::filesystem::path_user();
			for(nana::filesystem::file_iterator i(str); i != end; ++i)
			{
				if((false == i->directory) || (i->name.size() && i->name[0] == '.')) continue;
				if(tree_.insert(nodes_.home, i->name, i->name, kind::filesystem))
					break;
			}

			for(nana::filesystem::file_iterator i(STR("/")); i != end; ++i)
			{
				if((false == i->directory) || (i->name.size() && i->name[0] == '.')) continue;
				if(tree_.insert(nodes_.filesystem, i->name, i->name, kind::filesystem))
					break;
			}

			tree_.ext_event().expand = nana::make_fun(*this, &filebox_implement::_m_tr_expand);
			tree_.ext_event().selected = nana::make_fun(*this, &filebox_implement::_m_tr_selected);
		}

		nana::string _m_resolute_path(nana::string& path)
		{
			std::size_t pos = path.find(STR('/'));
			if(pos != path.npos)
			{
				nana::string begstr = path.substr(0, pos);
				if(begstr == STR("FS.HOME"))
					path.replace(0, 7, nana::filesystem::path_user());
				else
					path.erase(0, pos);
				return begstr;
			}
			return nana::string();
		}

		void _m_load_path(const nana::string& path)
		{
			addr_.filesystem = path;
			if(addr_.filesystem.size() && addr_.filesystem[addr_.filesystem.size() - 1] != STR('/'))
				addr_.filesystem += STR('/');

			file_container_.clear();
			nana::filesystem::attribute fattr;
			nana::filesystem::file_iterator end;
			for(nana::filesystem::file_iterator i(path); i != end; ++i)
			{
				if((i->name.size() == 0) || (i->name[0] == STR('.')))
					continue;
				item_fs m;
				m.name = i->name;
				if(nana::filesystem::file_attrib(path + m.name, fattr))
				{
					m.bytes = fattr.bytes;
					m.directory = fattr.is_directory;
					m.modified_time = fattr.modified;
				}
				else
				{
					m.bytes = 0;
					m.directory = i->directory;
					nana::filesystem::modified_file_time(path + i->name, m.modified_time);
				}

				file_container_.push_back(m);

				if(m.directory)
					path_.childset(m.name, 0);
			}
			std::sort(file_container_.begin(), file_container_.end(), pred_sort_fs());
		}

		void _m_load_cat_path(nana::string path)
		{
			if((path.size() == 0) || (path[path.size() - 1] != STR('/')))
				path += STR('/');

			node_type beg_node = tree_.selected();
			while(beg_node && (beg_node != nodes_.home) && (beg_node != nodes_.filesystem))
				beg_node = tree_.get_owner(beg_node);
			
			nana::string head = nana::filesystem::path_user();
			if(path.size() >= head.size() && (path.substr(0, head.size()) == head))
			{//This is HOME
				path_.caption(STR("HOME"));
				if(beg_node != nodes_.home)
					tree_.selected(nodes_.home);
			}
			else
			{	//Redirect to '/'
				path_.caption(STR("FILESYSTEM"));
				if(beg_node != nodes_.filesystem)
					tree_.selected(nodes_.filesystem);
				head.clear();
			}

			if(head.size() == 0 || head[head.size() - 1] != STR('/'))
				head += STR('/');

			nana::filesystem::file_iterator end;
			for(nana::filesystem::file_iterator i(head); i != end; ++i)
			{
				if(i->directory)
					path_.childset(i->name, 0);
			}
			nana::string cat_path = path_.caption();
			if(cat_path.size() && cat_path[cat_path.size() - 1] != STR('/'))
				cat_path += STR('/');

			std::size_t beg = head.size();
			while(true)
			{
				std::size_t pos = path.find(STR('/'), beg);
				nana::string folder = path.substr(beg, pos != path.npos ? pos - beg: path.npos);
				if(folder.size() == 0) break;
				(cat_path += folder) += STR('/');
				(head += folder) += STR('/');
				path_.caption(cat_path);
				
				for(nana::filesystem::file_iterator i(head); i != end; ++i)
				{
					if(i->directory)
						path_.childset(i->name, 0);
				}

				if(pos == path.npos)
					break;
				beg = pos + 1;
			}
			_m_load_path(path);
			_m_list_fs();
		}

		bool _m_filter_allowed(const nana::string& name, bool is_dir, const nana::string& filter, const std::vector<nana::string>* extension) const
		{
			if(filter.size())
				if(name.find(filter) == filter.npos)
					return false;

			if((is_dir || 0 == extension) || (0 == extension->size())) return true;

			for(std::vector<nana::string>::const_iterator i = extension->begin(); i != extension->end(); ++i)
			{
				std::size_t pos = name.rfind(*i);
				if((pos != name.npos) && (name.size() == pos + i->size()))
					return true;
			}
			return false;
		}

		void _m_list_fs()
		{
			nana::string filter = filter_.caption();
			ls_file_.auto_draw(false);

			ls_file_.clear();
			ls_file_.resolver(fs_resolver());

			std::vector<nana::string>* ext_types = cb_types_.anyobj<std::vector<nana::string> >(cb_types_.option());

			for(std::vector<item_fs>::iterator i = file_container_.begin(); i != file_container_.end(); ++i)
			{
				if(_m_filter_allowed(i->name, i->directory, filter, ext_types))
					ls_file_.append(0, *i);
			}
			ls_file_.auto_draw(true);
		}

		void _m_finish(kind::t type, const nana::string& tar)
		{
			selection_.target = tar;
			selection_.type = type;
			close();
		}

		void _m_keychar(const eventinfo& ei)
		{
			if(ei.keyboard.key == nana::gui::keyboard::enter)
				_m_ok();
		}

		struct folder_creator
		{
			folder_creator(filebox_implement& fb, form & fm, textbox& tx)
				:	fb_(fb), fm_(fm), tx_path_(tx)
			{}

			void operator()()
			{
				nana::string path = tx_path_.caption();

				msgbox mb(fm_, STR("Create Folder"));
				mb.icon(msgbox::icon_warning);
				if(0 == path.size() || path[0] == STR('.') || path[0] == STR('/'))
				{
					mb<<STR("Please input a valid name for the new folder.");
					mb();
					return;
				}
				
				bool if_exist;
				if(false == nana::filesystem::mkdir(fb_.addr_.filesystem + path, if_exist))
				{
					if(if_exist)
						mb<<STR("The folder is existing, please rename it.");
					else
						mb<<STR("Failed to create the folder, please rename it.");
					mb();
					return;
				}
				fb_._m_load_cat_path(fb_.addr_.filesystem);
				fm_.close();
			}

			filebox_implement& fb_;
			form& fm_;
			textbox & tx_path_;
		};

		void _m_new_folder()
		{
			form fm(*this, API::make_center(*this, 300, 35));
			fm.caption(STR("Name the new folder"));

			textbox folder(fm, nana::rectangle(5, 5, 160, 25));
			folder.multi_lines(false);

			button btn(fm, nana::rectangle(170, 5, 60, 25));
			btn.caption(STR("Create"));
			btn.make_event<events::click>(folder_creator(*this, fm, folder));

			button btn_cancel(fm, nana::rectangle(235, 5, 60, 25));
			btn_cancel.caption(STR("Cancel"));
			btn_cancel.make_event<events::click>(destroy(fm));
			API::modal_window(fm);
		}

		bool _m_append_def_extension(nana::string& tar) const
		{
			std::size_t dotpos = tar.find_last_of(STR('.'));
			if(dotpos != tar.npos)
			{
				std::size_t pos = tar.find_last_of(STR("/\\"));
				if(pos == tar.npos || pos < dotpos)
					return false;
			}

			if(def_ext_.size())
			{
				if(def_ext_[0] != STR('.'))
					tar += STR('.');
				tar += def_ext_;
			}

			std::vector<nana::string> * exts = cb_types_.anyobj<std::vector<nana::string> >(cb_types_.option());
			if(0 == exts || exts->size() == 0)	return false;
			
			const nana::string & ext = exts->at(0);
			if(def_ext_[0] != STR('.'))
				tar += STR('.');
			tar += ext;
			return true;
		}
	private:
		void _m_sel_file(const eventinfo& ei)
		{
			std::vector<std::pair<std::size_t, std::size_t> > sel;
			ls_file_.selected(sel);
			if(sel.size() == 0)
				return;
			std::pair<std::size_t, std::size_t> index = sel[0];
			item_fs m;
			if(false == ls_file_.item(index.first, index.second, m))
				return;

			if(events::dbl_click::identifier == ei.identifier)
			{
				if(m.directory)
				{
					_m_load_cat_path(addr_.filesystem + m.name + STR("/"));
				}
				else
					_m_finish(kind::filesystem, addr_.filesystem + m.name);
			}
			else
			{
				if(false == m.directory)
				{
					selection_.target = addr_.filesystem + m.name;
					tb_file_.caption(m.name);
				}
			}
		}

		void _m_filter_changed()
		{
			_m_list_fs();
		}

		void _m_ok()
		{
			if(0 == selection_.target.size())
			{
				nana::string file = tb_file_.caption();
				if(file.size())
				{
					if(file[0] == STR('.'))
					{
						msgbox mb(*this, caption());
						mb.icon(msgbox::icon_warning);
						mb<<file<<std::endl<<STR("The filename is invalid.");
						mb();
						return;
					}
					nana::string tar;
					if(file[0] == STR('/'))
						tar = file;
					else
						tar = addr_.filesystem + file;

					bool good = true;
					nana::filesystem::attribute attr;
					if(nana::filesystem::file_attrib(tar, attr) == false)
					{
						if(_m_append_def_extension(tar))
							good = nana::filesystem::file_attrib(tar, attr);
						else
							good = false;
					}
					
					if(good && attr.is_directory)
					{
						_m_load_cat_path(tar);
						tb_file_.caption(STR(""));
						return;
					}
					
					if(io_read_)
					{
						if(good)
							_m_finish(kind::filesystem, tar);
					}
					else
					{
						if(good)
						{
							msgbox mb(*this, caption(), msgbox::yes_no);
							mb.icon(msgbox::icon_question);
							mb<<STR("The input file is existing, do you want to overwrite it?");
							if(msgbox::pick_no == mb())
								return;
						}
						_m_finish(kind::filesystem, tar);
					}
				}
			}
			else
				_m_finish(kind::filesystem, selection_.target);
		}

		void _m_tr_expand(window, node_type node, bool exp)
		{
			if(false == exp) return;
			kind::t node_kind = tree_.value(node);
			if(kind::filesystem == node_kind)
			{
				nana::string path = tree_.make_key_path(node, STR("/")) + STR("/");
				_m_resolute_path(path);

				nana::filesystem::file_iterator end;
				for(nana::filesystem::file_iterator i(path); i != end; ++i)
				{
					if((false == i->directory) || (i->name.size() && i->name[0] == '.')) continue;
					node_type child = tree_.insert(node, i->name, i->name, node_kind);
					if(child)
					{
						for(nana::filesystem::file_iterator u(path + i->name); u != end; ++u)
						{
							if(false == u->directory || (u->name.size() && u->name[0] == '.')) continue;
							tree_.insert(child, u->name, u->name, node_kind);
							break;
						}
					}
				}
			}
		}

		void _m_tr_selected(window, node_type node, bool set)
		{
			if(set && (tree_.value(node) == kind::filesystem))
			{
				nana::string path = tree_.make_key_path(node, STR("/")) + STR("/");
				_m_resolute_path(path);
				_m_load_cat_path(path);
			}
		}

		void _m_cat_selected(categorize<int>& cat, int&)
		{
			nana::string path = cat.caption();
			std::size_t pos = path.find(STR('/'));
			nana::string root = path.substr(0, pos);
			if(root == STR("HOME"))
				path.replace(0, 4, nana::filesystem::path_user());
			else if(root == STR("FILESYSTEM"))
				path.erase(0, 10);
			else
				throw std::runtime_error("Nana.GUI.Filebox: Wrong categorize path");

			if(path.size() == 0) path = STR("/");
			_m_load_cat_path(path);
		}

		void _m_cb_type_selected(combox& cb)
		{
			_m_list_fs();
		}
	private:
		bool io_read_;
		nana::string def_ext_;

		gird	gird_;
		categorize<int> path_;
		textbox		filter_;
		button	btn_folder_;
		treebox<kind::t> tree_;
		listbox	ls_file_;

		label lb_file_;
		textbox	tb_file_;
		combox	cb_types_;
		button btn_ok_, btn_cancel_;

		struct tree_node_tag
		{
			node_type home;
			node_type filesystem;
		}nodes_;

		std::vector<item_fs> file_container_;
		struct path_tag
		{
			nana::string filesystem;
		}addr_;

		struct selection_tag
		{
			kind::t type;
			nana::string target;
		}selection_;
	};//end class filebox_implement

#endif
	//class filebox
		struct filebox::implement
		{
			struct filter
			{
				nana::string des;
				nana::string type;
			};

			window owner;
			bool open_or_save;

			nana::string file;
			nana::string title;
			nana::string path;
			std::vector<filter> filters;
		};

		filebox::filebox(window owner, bool open)
			: impl_(new implement)
		{
			impl_->owner = owner;
			impl_->open_or_save = open;
			impl_->file[0] = 0;
#if defined(NANA_WINDOWS)
			nana::char_t buf[260];
			DWORD len = ::GetCurrentDirectory(260, buf);
			if(len >= 260)
			{
				nana::char_t * p = new nana::char_t[len + 1];
				::GetCurrentDirectory(len + 1, p);
				impl_->path = p;
				delete [] p;
			}
			else
				impl_->path = buf;
#endif
		}

		filebox::~filebox()
		{
			delete impl_;
		}

		void filebox::title(const nana::string& s)
		{
			impl_->title = s;
		}

		void filebox::init_path(const nana::string& s)
		{
			nana::filesystem::attribute attr;
			if(nana::filesystem::file_attrib(s, attr))
				if(attr.is_directory)
					impl_->path = s;
		}

		void filebox::add_filter(const nana::string& description, const nana::string& filetype)
		{
			implement::filter flt = {description, filetype};
			impl_->filters.push_back(flt);
		}

		nana::string filebox::file() const
		{
			return impl_->file;
		}

		bool filebox::operator()() const
		{
#if defined(NANA_WINDOWS)
			nana::char_t buffer[520];
			OPENFILENAME ofn;
			memset(&ofn, 0, sizeof ofn);
			ofn.lStructSize = sizeof(ofn);
			ofn.hwndOwner = reinterpret_cast<HWND>(impl_->owner);
			ofn.lpstrFile = buffer;
			ofn.lpstrFile[0] = '\0';
			ofn.nMaxFile = sizeof(buffer) / sizeof(nana::char_t) - 1;

			//Filter
			nana::string filter;
			if(impl_->filters.size())
			{
				for(std::vector<implement::filter>::iterator i = impl_->filters.begin(); i != impl_->filters.end(); ++i)
				{
					filter += i->des;
					filter += static_cast<nana::string::value_type>('\0');
					nana::string fs = i->type;
					std::size_t pos = 0;
					while(true)
					{
						pos = fs.find(STR(" "), pos);
						if(pos == fs.npos)
							break;
						fs.erase(pos);
					}
					filter += fs;
					filter += static_cast<nana::string::value_type>('\0');
				}
			}
			else
				filter = STR("ALl Files\0*.*\0");

			ofn.lpstrFilter = filter.c_str();
			ofn.lpstrTitle = (impl_->title.size() ? impl_->title.c_str() : 0);
			ofn.nFilterIndex = 0;
			ofn.lpstrFileTitle = NULL;
			ofn.nMaxFileTitle = 0;
			ofn.lpstrInitialDir = (impl_->path.size() ? impl_->path.c_str() : 0);
			//ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST;
			if(FALSE != (impl_->open_or_save ? ::GetOpenFileName(&ofn) : ::GetSaveFileName(&ofn)))
			{
				impl_->file = buffer;
				return true;
			}
#elif defined(NANA_LINUX)
			filebox_implement fb(impl_->owner, impl_->open_or_save, impl_->title);

			if(impl_->filters.size())
			{
				for(std::vector<implement::filter>::iterator i = impl_->filters.begin(); i != impl_->filters.end(); ++i)
				{
					nana::string fs = i->type;
					std::size_t pos = 0;
					while(true)
					{
						pos = fs.find(STR(" "), pos);
						if(pos == fs.npos)
							break;
						fs.erase(pos);
					}
					fb.add_filter(i->des, fs);
				}
			}
			else
				fb.add_filter(STR("All Files"), STR("*.*"));

			API::modal_window(fb);
			if(fb.selected())
			{
				impl_->file = fb.file();
				return true;
			}
#endif
			return false;
		}
	//end class filebox
}//end namespace gui
}//end namespace nana
