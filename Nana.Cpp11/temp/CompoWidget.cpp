#include <../temp/EditableForm.hpp>

#include <iostream>    // temp, for debugging
#include <fstream>     // temp, for debugging

FilePickBox::FilePickBox (nana::gui::widget &fm, 
						  const nana::string   &label,
						  const nana::string   &DefLayoutFileName )
				:	CompoWidget(fm, label, DefLayoutFileName) 
{
    _label.caption(caption());
    _label.text_align(nana::align::right  ); 

    fb_p.title(STR("Pick ") + caption());

	Pick.make_event	<nana::gui::events::click> ([&](){pick(FileName());}	); 

	_fileName.editable(true);

    InitMyLayout();
    SelectClickableWidget( _label);

    _fileName.ext_event().selected = ([this](nana::gui::combox&cb)
            { 
            //assert((  std::cerr<< "\nBefore calling Validate,in select FilePickBox._fileName: " , true  ));;
            //assert((  std::wcerr<< caption() << std::endl , true  ));;
                      
                //if( this->UserSelected() )   
                    this->Validate(this->_validate_only);
            } );
    _fileName.make_event <nana::gui::events::focus>([this](const nana::gui::eventinfo& ei)
        {  
            //assert((  std::cerr<< "\nBefore calling validating_only," << (ei.focus.getting ? "geting ":"lossing ") << "Focus: , FilePickBox._fileName: " , true  ));;
            //assert((  std::wcerr<< caption() << std::endl , true  ));;
            
            if (!ei.focus.getting) 
                          this->validate_only();
        });


    _user_selected=true;
}
    void FilePickBox::SetDefLayout       () 
    {
       SetDefLayout       (49); 
    }

    void FilePickBox::SetDefLayout       (unsigned lab) 
{
    std::stringstream lay;
    lay<<
        "vertical   <weight=1>    \n"
        "           <weight=20 <weight=3><   vertical weight="<<lab<<" <><label weight=15><>     ><weight=1>     \n"
        "					   <cbFL >       \n"
        "					   <pick weight=30>  \n"
        "					   <weight=3> 	>" 
        "            <weight=2>    \n"          ;  
    _DefLayout=lay.str();    //ResetDefLayout();
}


    void FilePickBox::AsignWidgetToFields() 
{
	_place.field("cbFL"        ) <<  _fileName ;
	_place.field("label"       ) << _label;
	_place.field("pick"        ) << Pick;
}
void FilePickBox::pick(const nana::string &file_tip)
{
    bool  vo{ true };
    std::swap(vo,_validate_only);
    select_file( fb_p, STR(""), file_tip,true);
    std::swap(vo,_validate_only);
}
void FilePickBox::select_file(nana::gui::filebox&  fb, const nana::string &action, const nana::string &file_tip, bool select_only)
{
    nana::string old_t;
    if (!action.empty())
        old_t=fb.title(action);
    fb.init_file(file_tip); 
	if(fb())  
	{	
        select_only ? FileNameOnly (fb.file()) : FileNameOpen (fb.file());
        _canceled= false;
    }
    else 
        _canceled= true;
    if (!action.empty()) // revisar !!
        fb.title(old_t);
 }


OpenSaveBox::OpenSaveBox (nana::gui::widget &fm, 
						  const nana::string   &label,
						  const nana::string   &DefLayoutFileName )
				:	FilePickBox(fm, label, DefLayoutFileName) 
{
    fb_o.title(STR("Open ") + caption());
    fb_s.title(STR("Save ") + caption());

    Open.make_event	<nana::gui::events::click> ([&](){open(FileName());}	);
	Save.make_event	<nana::gui::events::click> ([&](){save(FileName());}    );

    InitMyLayout();
}
    void OpenSaveBox::SetDefLayout       () 
{
    _DefLayout= 
        "vertical   <weight=1>    \n"
        "           <weight=20 <weight=3><   vertical weight=49 <><label weight=15><>     ><weight=1>     \n"
        "		               <proj_buttons weight=74 gap=1>     \n"
        "					   <cbFL >       \n"
        "					   <pick weight=30>  \n"
        "					   <weight=3> 	>" 
        "            <weight=2>    \n"          ;  
}
    void OpenSaveBox::AsignWidgetToFields() 
{
	_place.field("proj_buttons") << Open << Save;
    //FilePickBox::AsignWidgetToFields() ;
}
	
void OpenSaveBox::open(const nana::string &file_tip)
{
    bool us{ false }, vo{ false };
    std::swap(vo,_validate_only);
	select_file( fb_o, STR(""), file_tip,false);
    std::swap(vo,_validate_only);
}
void OpenSaveBox::save(const nana::string &file_tip,  const nana::string &action)
{
    bool  vo{ true };
    std::swap(vo,_validate_only);
	select_file( fb_s, action , file_tip, true);
    std::swap(vo,_validate_only);
}

