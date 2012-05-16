require 'rails/generators'

class AdminBootstrapTask
  TEMPLATE_PATH = File.expand_path(File.join(File.dirname(__FILE__), '..', 'generators', 'admin_bootstrap', 'templates'))
  RESOURCES = %w(app/assets app/controllers/admin_controller.rb app/controllers/admin/dashboard_controller.rb app/helpers/admin_helper.rb app/models/admin.rb app/views/shared/admin app/views/layouts/admin.html.haml app/views/layouts/admin.html.erb app/views/admin/dashboard/index.html.haml app/views/admin/dashboard/index.html.erb)

  def self.prepare options = {}
    use_haml = !options[:erb] && (options[:haml] or (!(options[:haml] === false) and (Gem.available?('haml') and Gem.available?('haml-rails'))))

    routes_file = File.join(Rails.root, 'config', 'routes.rb')
    routes_content = File.read(routes_file)
    route_rule = "\n  namespace :admin do root :to => 'dashboard#index' end\n"

    unless routes_content.match(route_rule.strip)
      File.open(routes_file, 'w') do |f|
        new_routes = routes_content.gsub(/\.routes\.draw do(?:\s*\|map\|)?\s*$/) do |after|
          after + route_rule
        end
        f.write(new_routes)
      end
    end

    formtastic_initializer = File.join(Rails.root, 'config', 'initializers', 'formtastic.rb')
    formtastic_rule = "\nFormtastic::Helpers::FormHelper.builder = FormtasticBootstrap::FormBuilder\n"

    unless File.exists?(formtastic_initializer)
      Rails::Generators.invoke("formtastic:install")
    end
    unless File.read(formtastic_initializer).match(formtastic_rule.strip)
      File.open(formtastic_initializer, 'a') do |fi|
        fi.puts(formtastic_rule)
      end
    end

    files = RESOURCES.collect do |resource|
      path = File.join(TEMPLATE_PATH, resource)
      if File.directory?(path)
        Dir.glob(File.join(path, '**', '**', '**', '**')).collect do |item|
          item.gsub(Regexp.new(TEMPLATE_PATH + '/?'), '')
        end
      else
        resource
      end
    end.flatten.reject do |file|
      file.match(use_haml ? /\.erb$/ : /\.haml$/) or File.directory?(File.join(TEMPLATE_PATH, file))
    end

    files.each do |file|
      begin
        FileUtils.mkdir_p File.join(Rails.root, File.dirname(file)) unless File.exists?(file)
        unless File.exists?(File.join(Rails.root, file))
          FileUtils.mkdir_p(File.join(Rails.root, File.dirname(file)))
          FileUtils.cp File.join(TEMPLATE_PATH, file), File.join(Rails.root, file)
        end
      rescue
        puts "An error occurred while copying file #{file}"
      end
    end
  end

end

namespace :admin_bootstrap do
  desc "Copy all necessary assets and files for admin bootstrap, try to guess which views (haml or erb) to use"
  task :initialize => :environment do
    AdminBootstrapTask.prepare
  end

  namespace :initialize do
    desc "Copy all necessary assets and files for admin bootstrap using only haml views"
    task :haml => :environment do
      AdminBootstrapTask.prepare :haml => true
    end

    desc "Copy all necessary assets and files for admin bootstrap using only erb views"
    task :erb => :environment do
      AdminBootstrapTask.prepare :erb => true
    end
  end
end
